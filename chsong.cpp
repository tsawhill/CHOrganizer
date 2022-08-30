#include "chsong.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QProgressDialog>

const QString &CHSong::getPath() const{
    return path;
}

void CHSong::setPath(const QString &newPath){
    path = newPath;
}

CHSong::CHSong(QString path){
    this->path = path;


    QFile input_file(path);

    /*  Checking file size of song.ini
     *  In my library, most were < 1000, but max was around 1300.
     *  Giving generous filesize limit of 3000
     */
    if(QFileInfo(input_file).size() < 3000){
        if(!input_file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0, "Error reading files. Do you have permission to read this directory?", input_file.errorString());
        }

        QTextStream input_iterator(&input_file);

        //Store song info from song.ini into map.
        while(!input_iterator.atEnd()){
            QString line = input_iterator.readLine();
            QStringList info = line.split("="); //Storing ALL song data in case of future use

            //Line 1 will contain [Song] which will not split properly.
            //We will trim the song data to avoid any extra spaces before/after the String
            if(info.size() != 1){
               song_data.insert(info.at(0).trimmed(), info.at(1).trimmed());
             }
        }
        input_file.close();
    }


}

QString CHSong::getArtist(){
    if(song_data.contains("artist")){
        QString artist_name = song_data.value("artist");
        if(artist_name == "")
            return "Unspecified";
        else
            return artist_name;
    }
    else
        return "Unspecified";
}

QString CHSong::getAlbum(){
    if(song_data.contains("album")){
        QString album_title = song_data.value("album");
        if(album_title == "")
            return "Unspecified";
        else
            return album_title;
    }
    else
        return "Unspecified";
}

QString CHSong::getTitle(){
    if(song_data.contains("name")){
        return song_data.value("name");
    }
    else
        return "error";
}

QList<CHSong> CHSong::scan_directory(QString dir){
    //Create list to store song.ini paths of all songs found
    QList<CHSong> songs;

    //Find all song.ini files in the song directory and add them to songs
    QDirIterator it(dir, QStringList() << "song.ini", QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()){
        songs.append(CHSong(it.next()));
    }

    return songs;
}

QStringList CHSong::update_artists(QList<CHSong> songs){
    //Add artists to StringList for all songs, and remove duplicates.
    QStringList artists = {};
    for(CHSong i : songs){
        artists.append(i.getArtist());
    }
    artists.removeDuplicates();

    return artists;
}

void CHSong::copy_songs(QString library_path, QProgressDialog &progress, QList<CHSong> songs){
    //Create directory for the copied files
    QString processed_path = library_path + "/CHOrganizer/";
    QDir processed_dir(processed_path);

    int num_copied = 0;

    //If the CHOrganizer directory exists, give error
    //In the future, give the option to overwrite existing directory
    if(processed_dir.exists()){
        QString err_msg = "The path " + processed_path + " already exists. Please delete it before processing.";
        QMessageBox::critical(
          NULL,
          "CHOrganizer",
          err_msg);
    }
    else{
        if(processed_dir.mkpath(processed_path)){
            for(CHSong x : songs){
                //Update progress bar and create QDir from song path
                progress.setValue(num_copied++);
                QString song_path = processed_dir.absolutePath() + "/" + x.getArtist() + "/" + x.getAlbum() + "/" + x.getTitle() + "/";
                QDir song_dir(song_path);

                //If the song directory already exists, give error message
                //In the future, possibly increment a number after file name instead of error
                if(song_dir.exists()){
                    QString err_msg = "The path " + song_path + " already exists.\nNext version will have logic to handle duplicate songs.\nSong at: " + x.getPath() + " will not be processed.";
                    QMessageBox::critical(
                      NULL,
                      "CHOrganizer",
                      err_msg);
                }
                else{
                    if(processed_dir.mkpath(song_path)){
                        //File names that relate to clone hero
                        //May be missing some as I sampled my own library.
                        QStringList filenames = {"1x.mid",
                                    "1x.chart",
                                    "2x.mid",
                                    "2x.chart",
                                    "album.png",
                                    "background.png",
                                    "drums_1.ogg",
                                    "drums_2.ogg",
                                    "drums_3.ogg",
                                    "drums_4.ogg",
                                    "expert+.mid",
                                    "expert+.chart",
                                    "notes.mid",
                                    "notes.chart",
                                    "notes.eof",
                                    "guitar.ogg",
                                    "preview.ogg",
                                    "rhythm.ogg",
                                    "song.ini",
                                    "song.ogg",
                                    "song.mp3",
                                    "vocals.ogg"};

                        QString original_path(QFileInfo(x.getPath()).absolutePath());

                        //Copy the files
                        for(QString x : filenames){
                            QString original_file(original_path + "/" + x);
                            QString new_file(song_path + "/" + x);
                            if(QFile(original_file).exists()){
                                QFile::copy(original_file, new_file);
                            }
                        }

                    }
                    //If we cannot create the directory, give error message
                    else{
                        QString err_msg = "Failed to create path: " + song_path + " . Do you have write permissions in this directory?";
                        QMessageBox::critical(
                          NULL,
                          "CHOrganizer",
                          err_msg);
                    }
                }

        }
            }
            //If we cannot create the path, give error message
            else{
                QString err_msg = "Failed to create path: " + processed_path + " . Do you have write permissions in this directory?";
                QMessageBox::critical(
                  NULL,
                  "CHOrganizer",
                  err_msg);
            }
        }
}