#include "fileindexer.h"
#include <QDebug>

FileIndexer::FileIndexer()
{

}

FileIndexer::~FileIndexer()
{

}

void FileIndexer::buildIndex(QDir *dir) {
    QMap<QString,QStringList> *word_is_in_file = new QMap<QString,QStringList>;
    QMap<QString, QString> *file_and_id = new QMap<QString, QString>;
    QSqlQuery q;
    QMimeDatabase qmd;
    QStringList useless_words = QStringList();
    if(q.exec("SELECT * FROM useless_word")) {
        while(q.next()) {
            useless_words.append(q.value(1).toString());
        }

        QDirIterator it(dir->absolutePath(), QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString file_path = it.next();
            QFileInfo file = QFileInfo(file_path);
            QString file_type = qmd.mimeTypeForFile(file).name();
            if(file_type.contains(QRegExp("(audio/|text/|image/|application/pdf|inode/directory)"))){
                if(q.exec("SELECT * FROM indexed_file WHERE file_path =\"" + file_path + "\"")) {
                    QDateTime index_date = QDateTime::currentDateTime();
                    bool isIndexed = false;
                    while(q.next()) {
                        //On récupère la date d'indexation du fichier pour la comparer à la date de dernière modification du fichier.
                        //Si l'index est plus récent que la modification, on ne traitera pas le fichier
                        index_date = buildDateTimeFromQString(q.value(2).toString());
                        isIndexed = true;
                    }
                    //On travaille sur le fichier s'il n'est pas indexé, partie gauche du OU
                    if((index_date > file.lastModified() && !isIndexed) || (isIndexed && index_date < file.lastModified())) {
                        qDebug() << file.baseName() << " va être indexé";
                        QStringList file_content;

                        //Chargement du fichier en fonction de son format
                        if(file_type.contains(QRegExp("text/"))) {
                            file_content = loadTextContent(file_path, useless_words);
                        } else if (file_type.contains(QRegExp("audio/"))){
                            file_content = loadAudioContent(file_path, useless_words);
                        } else if (file_type.contains(QRegExp("image/"))) {
                            file_content = loadImageContent(file_path, useless_words);
                        } else if (file_type.contains(QRegExp("application/pdf"))) {
                            //file_content = loadPdfContent(file_path, useless_words);
                        } else if (file_type.contains(QRegExp("inode/directory"))) {
                            //Pour les dossiers, on ne travaille uniquement que sur leurs noms
                            file_content = QStringList(file.baseName());
                        } else {
                            //Liste vide de texte pour les formats non supportés
                            file_content = QStringList();
                        }

                        foreach(QString word, file_content) {
                            if(word_is_in_file->find(word) == word_is_in_file->end()) {
                                word_is_in_file->insert(word, QStringList(file_path));
                            } else {
                                QStringList files = word_is_in_file->value(word);
                                files.append(file_path);
                                word_is_in_file->insert(word, files);
                            }
                        }

                        if(q.exec("SELECT * FROM indexed_file WHERE file_path='" + file_path + "'") && file_type.contains(QRegExp("(audio/|text/|image/|application/pdf|inode/directory)"))) {
                            QString indexed_file_id = "";
                            while(q.next()) {
                                indexed_file_id = q.value(0).toString();
                            }
                            index_date = QDateTime::currentDateTime();
                            QString index_date_string = QString::number(index_date.date().year()) + "-" + QString::number(index_date.date().month()) + "-" + QString::number(index_date.date().day()) + " " + QString::number(index_date.time().hour()) + ":" + QString::number(index_date.time().minute()) + ":" + QString::number(index_date.time().second()) + ":" + QString::number(index_date.time().msec());
                            if(indexed_file_id == "") {
                                if(q.exec("INSERT INTO indexed_file (file_path, index_date) VALUES ('" + file_path+ "','" + index_date_string  +"')")) {
                                    indexed_file_id = q.lastInsertId().toString();
                                    file_and_id->insert(file_path, indexed_file_id);
                                } else {
                                    qDebug() << "Fail de : INSERT INTO indexed_file (file_path, index_date) VALUES ('";
                                }
                            } else {
                                if(q.exec("UPDATE indexed_file SET index_date ='" + index_date_string + "' WHERE indexed_file_id='" + indexed_file_id + "'")) {
                                    file_and_id->insert(file_path, indexed_file_id);
                                } else {
                                    qDebug() << "Fail de : UPDATE indexed_file SET index_date =";
                                }
                            }

                        } else {
                            qDebug() << "Fail de : SELECT * FROM word WHERE file_path" << file_path;
                        }

                    } else {
                        qDebug() << file.baseName() << " ne va pas être indexé";
                    }
                } else {
                    qDebug() << "La recherche d'un fichier dans indexed_file a échoué";
                }
            }
        }
        if(!word_is_in_file->isEmpty()) {
            //On sauvegarde la QMap
            for(QMap<QString,QStringList>::iterator it = word_is_in_file->begin();
                it != word_is_in_file->end();
                it++)
            {
                if(q.exec("SELECT word_id FROM word WHERE value='" + it.key() + "'")){
                    QString word_id = "";
                    while(q.next()) {
                        word_id = q.value(0).toString();
                    }
                    if(word_id == "") {
                        if(q.exec("INSERT INTO word (value) VALUES ('"+ it.key() +"')")) {
                            word_id = q.lastInsertId().toString();
                        } else {
                            word_id = QString::number(-1);
                        }
                    }

                    //Ajout du lien avec les fichiers
                    if(word_id != "-1") {
                        foreach(QString file_word, it.value()) {
                            if(q.exec("SELECT * FROM word_is_in_file WHERE word_id_fk='" + word_id + "' AND indexed_file_id_fk='"+ file_and_id->value(file_word) + "'")) {
                                QString word_is_in_file_id = "";
                                while(q.next()) {
                                    word_is_in_file_id = q.value(0).toString();
                                }

                                if(word_is_in_file_id == "") {
                                   if(q.exec("INSERT INTO word_is_in_file (word_id_fk, indexed_file_id_fk) VALUES ('" + word_id + "', '" + file_and_id->value(file_word) + "')")) {

                                   } else {
                                       qDebug() << "Fail d'ajout dans word_is_in_file";
                                   }
                                }
                            } else {
                                qDebug() << "Fail de SELECT * FROM word_is_in_file WHERE word_id_fk=";
                            }
                        }
                    }
                } else {
                    qDebug() << "Fail: SELECT * FROM word WHERE value";
                }
            }
        }
    } else {
        qDebug() << "Echec de chargement des mots inutiles";
    }
}

QDateTime FileIndexer::buildDateTimeFromQString(QString d) {
    QStringList sep_date_time = d.split(' ');
    QStringList split_date = sep_date_time[0].split('-');
    QStringList split_time = sep_date_time[1].split(':');
    QString year = split_date[0];
    QString month = split_date[1];
    QString day = split_date[2];
    QString hour = split_time[0];
    QString minute = split_time[1];
    QString second = split_time[2];
    QString msecond = split_time[3];

    return QDateTime(QDate(year.toInt(), month.toInt(), day.toInt()), QTime(hour.toInt(), minute.toInt(), second.toInt(), msecond.toInt()));
}

QStringList FileIndexer::loadTextContent(QString file_path, QStringList useless_words) {
    QStringList res = QStringList();
    QFile text_file(file_path);
    if (text_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&text_file);
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            line = removePunctuation(line);
            line = removeAccentsAndToLower(line);
            res = line.split(" ");
            res.removeAll("");
            foreach(QString word, res) {
                if(useless_words.contains(word)) {
                    res.removeAll(word);
                }
            }
        }
    }
    text_file.close();
    return res;
}

QStringList FileIndexer::loadAudioContent(QString file_path, QStringList useless_words) {
    QStringList res = QStringList();
    AudioMetadata am = AudioMetadata(file_path);
    res.append(removeAccentsAndToLower(removePunctuation(am.getAlbum())).split(" "));
    res.append(removeAccentsAndToLower(removePunctuation(am.getArtist())).split(" "));
    res.append(removeAccentsAndToLower(removePunctuation(am.getGenre())).split(" "));
    res.append(removeAccentsAndToLower(removePunctuation(am.getTitle())).split(" "));
    res.append(removeAccentsAndToLower(removePunctuation(QString::number(am.getYear()))).split(" "));
    foreach(QString word, res) {
        if(useless_words.contains(word)) {
            res.removeAll(word);
        }
    }
    return res;
}

QStringList FileIndexer::loadImageContent(QString file_path, QStringList useless_words) {
    ImageMetadata im = ImageMetadata(file_path);
    QStringList meta = QStringList(im.getMetadata());
    QStringList res = QStringList();
    foreach (QString line, meta) {
        line = removePunctuation(line);
        line = removeAccentsAndToLower(line);
        qDebug() << line;
        res = line.split(" ");
        res.removeAll("");
        foreach(QString word, res) {
            if(useless_words.contains(word)) {
                res.removeAll(word);
            }
        }
    }
    return res;
}

QStringList FileIndexer::loadPdfContent(QString file_path, QStringList useless_words) {
    PdfMetadata pdfm = PdfMetadata(file_path);
    QStringList res = pdfm.getCutText();
    foreach(QString line, res) {
        qDebug() << line;
    }
    res = QStringList();
    return res;
}

QString FileIndexer::removeAccentsAndToLower(QString string) {
    string = string.toLower();
    string = string.replace(QRegExp("(à|â|ä)"), "a");
    string = string.replace(QRegExp("(è|é|ê|ë)"), "e");
    string = string.replace(QRegExp("(ô|ö)"), "o");
    string = string.replace(QRegExp("(û|ü|ù)"), "u");
    string = string.replace(QRegExp("(ï|î)"), "i");

    return string;
}

QString FileIndexer::removePunctuation(QString string) {
    string = string.replace("(", "");
    string = string.replace(")", "");
    string = string.replace(":", "");
    string = string.replace(".", "");
    string = string.replace("!", "");
    string = string.replace("?", "");
    string = string.replace(",", "");
    string = string.replace(";", "");
    string = string.replace("\\", "");
    string = string.replace("_", "");
    string = string.replace("'", " ");
    string = string.replace("\"", "");
    return string;
}
