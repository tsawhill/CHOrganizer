#ifndef CHSONG_H
#define CHSONG_H
#include <QtCore>
#include <QProgressDialog>

class CHSong{
    private:
    QString path;
    public:
        static void copy_songs(QString library_path, QProgressDialog &progress, QList<CHSong> songs);
        static QList<CHSong> scan_directory(QString dir);
        static QStringList update_artists(QList<CHSong> songs);

        QMap<QString,QString> song_data;
        CHSong(QString path);
        QString getArtist();
        QString getAlbum();
        QString getTitle();

        const QString &getPath() const;
        void setPath(const QString &newPath);
};


#endif // CHSONG_H
