/**
 * Mupen64PlusAE, an N64 emulator for the Android platform
 * 
 * Copyright (C) 2013 Paul Lamb
 * 
 * This file is part of Mupen64PlusAE.
 * 
 * Mupen64PlusAE is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * Mupen64PlusAE is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with Mupen64PlusAE. If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 * Authors: littleguy77
 */
package paulscode.android.mupen64plusae.task;

import java.io.File;
import java.util.List;

import paulscode.android.mupen64plusae.persistent.ConfigFile;
import paulscode.android.mupen64plusae.persistent.ConfigFile.ConfigSection;
import paulscode.android.mupen64plusae.util.FileUtil;
import paulscode.android.mupen64plusae.util.RomDetail;
import paulscode.android.mupen64plusae.util.RomHeader;
import android.os.AsyncTask;
import android.text.TextUtils;
import android.util.Log;

public class CacheRomInfoTask extends AsyncTask<Void, ConfigSection, ConfigFile>
{
    public interface CacheRomInfoListener
    {
        public void onCacheRomInfoProgress( ConfigSection section );
        public void onCacheRomInfoFinished( ConfigFile file );
    }
    
    public CacheRomInfoTask( List<File> files, String configPath, String artDir, CacheRomInfoListener listener )
    {
        if( files == null )
            throw new IllegalArgumentException( "File list cannot be null" );
        if( TextUtils.isEmpty( configPath ) )
            throw new IllegalArgumentException( "Config file path cannot be null or empty" );
        if( TextUtils.isEmpty( artDir ) )
            throw new IllegalArgumentException( "Art directory cannot be null or empty" );
        if( listener == null )
            throw new IllegalArgumentException( "Listener cannot be null" );
        
        mFiles = files;
        mConfigPath = configPath;
        mArtDir = artDir;
        mListener = listener;
    }
    
    private final List<File> mFiles;
    private final String mConfigPath;
    private final String mArtDir;
    private final CacheRomInfoListener mListener;
    
    @Override
    protected ConfigFile doInBackground( Void... params )
    {
        final ConfigFile config = new ConfigFile( mConfigPath );
        config.clear();
        
        for( final File file : mFiles )
        {
            String md5 = RomDetail.computeMd5( file );
            RomDetail detail = RomDetail.lookupByMd5( md5 );
            if( detail == null )
            {
                // MD5 not in the database; lookup by CRC instead
                String crc = new RomHeader( file ).crc;
                RomDetail[] romDetails = RomDetail.lookupByCrc( crc );
                if( romDetails.length == 0 )
                {
                    // CRC not in the database; create best guess
                    Log.w( "RomCache", "No meta-info entry found for ROM " + file.getAbsolutePath() );
                    Log.i( "RomCache", "Constructing a best guess for the meta-info" );
                    String goodName = file.getName().split( "\\." )[0];
                    detail = RomDetail.createAssumption( md5, crc, goodName );
                }
                else if( romDetails.length > 1 )
                {
                    // CRC in the database more than once; let user pick best match
                    // TODO Implement popup selector
                    Log.w( "RomCache", "Multiple meta-info entries found for ROM " + file.getAbsolutePath() );
                    Log.i( "RomCache", "Defaulting to first entry" );
                    detail = romDetails[0];
                }
                else
                {
                    // CRC in the database exactly once; use it
                    detail = romDetails[0];
                }
            }
            String artPath = mArtDir + "/" + detail.artName;
            config.put( md5, "refMd5", detail.md5 );
            config.put( md5, "goodName", detail.goodName );
            config.put( md5, "romPath", file.getAbsolutePath() );
            config.put( md5, "artPath", artPath );
            FileUtil.downloadFile( detail.artUrl, artPath );
            
            this.publishProgress( config.get( md5 ) );
        }
        config.save();
        return config;
    }
    
    @Override
    protected void onProgressUpdate( ConfigSection... values )
    {
        mListener.onCacheRomInfoProgress( values[0] );
    }
    
    @Override
    protected void onPostExecute( ConfigFile result )
    {
        mListener.onCacheRomInfoFinished( result );
    }
}