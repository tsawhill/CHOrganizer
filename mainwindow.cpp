#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chsong.h"
#include <algorithm>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>


//Create list of songs and library path
QList<CHSong> songs;
QString library_path;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_directory_released()
{
    //Get library path from user
    library_path = QFileDialog::getExistingDirectory(this, tr("Please select your Clone Hero song library."),
                                                 "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    //Scan for song.ini in this directory and subdirectories
    songs = CHSong::scan_directory(library_path);

    //Add unique artists to the Artists list in UI
    ui->list_artists->addItems(CHSong::update_artists(songs));
}


void MainWindow::on_list_artists_itemSelectionChanged()
{
    //Clear populated songs and album list when new artist selected
    ui->list_songs->clear();
    ui->list_albums->clear();

    //Create empty QStringList to store album data
    QStringList albums = {};

    //Add every song's album to the QStringList, then remove duplicates
    for(CHSong i : songs){
        if(i.getArtist() == ui->list_artists->currentItem()->text()){
            albums.append(i.getAlbum());
        }
    }
    albums.removeDuplicates();

    //Update the ui with the album list
    ui->list_albums->addItems(albums);
}

void MainWindow::on_list_albums_itemSelectionChanged()
{
    //Clear list of songs when new album is selected
    ui->list_songs->clear();

    QString artist = ui->list_artists->currentItem()->text();
    QString album = ui->list_albums->currentItem()->text();
    QStringList album_songs = {}; //QStringList to hold the song data

    //If song and artist match, add to the QStringList
    for(CHSong i : songs){
        if(i.getArtist() == artist && i.getAlbum() == album){
            album_songs.append(i.getTitle());
        }
    }

    //Insert data from QStringList into ui
    ui->list_songs->addItems(album_songs);
}

void MainWindow::on_button_process_clicked()
{
    //Get number of songs for progress dialog
    int num_songs = songs.size();

    //Create dialog and copy songs
    QProgressDialog progress("Copying song data...\nThis may take a few minutes depending on library size.", "Cancel", 0, num_songs, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);
    CHSong::copy_songs(library_path, progress, songs);

    //Display complete message
    progress.setValue(num_songs);
    QMessageBox messageBox;
    messageBox.information(this, "CHOrganizer", "Complete!");
}


void MainWindow::on_button_edit_song_clicked()
{
    QMessageBox messageBox;
    messageBox.information(this, "CHOrganizer", "Coming soon!");
}

