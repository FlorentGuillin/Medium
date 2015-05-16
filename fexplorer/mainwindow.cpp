#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fs_model = new QFileSystemModel();
    fs_model->setRootPath(QDir::currentPath());
    fs_model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    ui->fsTreeView->setModel(fs_model);
    //Cache la colonne Size, inutile pour des dossiers
    ui->fsTreeView->hideColumn(1);
    //Cache la colonne Type, inutile vu que des dossiers
    ui->fsTreeView->hideColumn(2);
    //Cache la colonne DateModified, ça prend de la place pour rien?
    ui->fsTreeView->hideColumn(3);
    curr_dir = new QDir("/");
    displayDir("/");
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

/* affiche le répertoire dispo dans curr_dir et le prend comme racine */
void MainWindow::displayDir(QString dir_path)
{
    QDir *new_dir = new QDir(dir_path);
    if(new_dir->exists())
    {
        if(curr_dir != 0)
            delete curr_dir;

        curr_dir = new_dir;
    }

    ui->pathLine->setText("Répertoire courant : " + (curr_dir->dirName().isEmpty() ? "/" : curr_dir->dirName()));
    ui->pathLine->setToolTip(curr_dir->absolutePath());
    ui->fsTreeView->setRootIndex(fs_model->index(curr_dir->absolutePath()));


    //Si le dossier courant est la racine, on désactive le bouton précédent (impossible de trouver un dossier parent)
    if(QString::compare(curr_dir->path(), new QString("/"), Qt::CaseInsensitive) == 0){
        ui->parentDirButton->setEnabled(false);
    } else {
        ui->parentDirButton->setEnabled(true);
    }
}

//Affiche les bookmarks de l'utilisateur
void MainWindow::displayBookmarks() {
    if(ui->bookmark_listWidget->count() > 0) {
        ui->bookmark_listWidget->clear();
    }

    //Récupération des bookmarks
    QSqlQuery *q = new QSqlQuery(db);
    q->setForwardOnly(true);

    if(q->exec("SELECT * FROM bookmark")) {
        QIcon bookmark_icon = QIcon(":/icons/icons/bookmark.png");
        while(q->next()) {
            QListWidgetItem *qlwi = new QListWidgetItem(bookmark_icon, q->value(3).toString(), ui->bookmark_listWidget);
        }
    } else {
        QMessageBox::warning(this, "Erreur de récupération des bookmarks", " Medium n'a pas réussi à récupérer les bookmarks : \n\"" %
                              q->lastError().driverText() %
                              " : " %
                              q->lastError().databaseText() %
                              (q->lastError().nativeErrorCode().isEmpty() ? "" : " : " + q->lastError().nativeErrorCode()));
    }
}

bool MainWindow::connectSQLite() {
    bool res;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./medium.db");
    if((res = db.open())) {
        QSqlQuery q;
        if((res = q.exec("CREATE TABLE IF NOT EXISTS bookmark (bookmark_id INTEGER PRIMARY KEY AUTOINCREMENT, filter TEXT, search_directory TEXT, search_name TEXT)"))) {
            if((res = q.exec("CREATE TABLE IF NOT EXISTS file (file_id INTEGER PRIMARY KEY AUTOINCREMENT, bookmark_id_fk INTEGER, file_path TEXT, FOREIGN KEY(bookmark_id_fk) REFERENCES bookmark(bookmark_id))"))) {

            } else {
                QMessageBox::critical(this, "Erreur de création de table", " Medium n'a pas réussi à créer la table File : \n\"" %
                                      q.lastError().driverText() %
                                      " : " %
                                      q.lastError().databaseText() %
                                      (db.lastError().nativeErrorCode().isEmpty() ? "" : " : " + db.lastError().nativeErrorCode()) %
                                      "\"\nL'application va maintenant se fermer.");
            }
        } else {
            QMessageBox::critical(this, "Erreur de création de table", " Medium n'a pas réussi à créer la table Bookmark : \n\"" %
                                  q.lastError().driverText() %
                                  " : " %
                                  q.lastError().databaseText() %
                                  (db.lastError().nativeErrorCode().isEmpty() ? "" : " : " + db.lastError().nativeErrorCode()) %
                                  "\"\nL'application va maintenant se fermer.");
        }
    } else {
        //On affiche l'erreur de connexion à la base, l'appli est fermé depuis main.cpp
        QMessageBox::critical(this, "Erreur de connexion à la base de données", " Medium n'a pas réussi à se connecter à la base de données : \n\"" %
                              db.lastError().driverText() %
                              " : " %
                              db.lastError().databaseText() %
                              (db.lastError().nativeErrorCode().isEmpty() ? "" : " : " + db.lastError().nativeErrorCode()) %
                              "\"\nL'application va maintenant se fermer.");
    }

    return res;
}

/* slot pour quand on clique sur le bouton: ouverture d'un explorateur de fichier afin de sélectionner un dossier */
void MainWindow::on_changeDirButton_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Ouvrir Dossier"), ui->pathLine->text());

    displayDir(dirName);
}

/* slot pour quand on double-clique sur un dossier de ui->fsTreeView*/
void MainWindow::on_fsTreeView_doubleClicked(const QModelIndex &index)
{
    QString next_path = fs_model->filePath(index);

    displayDir(next_path);
}

void MainWindow::on_parentDirButton_clicked()
{
    QString s = curr_dir->path();
    s = s.left(s.lastIndexOf("/"));
    s = (s.isEmpty() == true ? "/" : s);
    displayDir(s);
}

QFileInfoList MainWindow::getAllFilesRecursively(QDir* dir){
    QFileInfoList res = QFileInfoList();

    QStringList name_filter = QStringList("*");
    QFileInfoList files = dir->entryInfoList(name_filter, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    QMimeDatabase qmd;

    foreach (QFileInfo file, files){
        if (file.isDir()){
            res.append(getAllFilesRecursively(new QDir(file.filePath())));

        }else{
            QString mimetype = qmd.mimeTypeForFile(file).name();
            if(mimetype.contains(QRegularExpression("text/"))) {
                res.append(file);
            }
        }
    }

    return res;
}

void MainWindow::on_filterButton_clicked()
{
    QString search_filter = ui->filter_lineEdit->text();
    if(!search_filter.isEmpty()) {
        //Récupération de tous les fichiers contenu sous le répertoire courant
        QFileInfoList files = getAllFilesRecursively(curr_dir);
        foreach(QFileInfo file, files) {

            //Récupération du MIME Type pour savoir si on traite ou pas le fichier
            QMimeDatabase qmd;
            QString mimetype = qmd.mimeTypeForFile(file).name();

            //Teste si le fichier ou son nom contient le critère de recherche à l'aide de grep dans les fichiers textes
            //TODO: Tester si cette manière de procéder est cross-platform tout de même, j'ai des gros doutes #Windaube
            if(mimetype.contains(QRegularExpression("text/"))) {

                QProcess *process = new QProcess;
                process->start("bash", QStringList() << "-c" << "cat " + file.filePath()  + " | grep " + search_filter);
                process->waitForBytesWritten();
                process->waitForFinished();

                //Si le grep renvoie une chaîne vide ET que le nom du fichier ne comporte pas le critère de recherche, le fichier est retiré de la liste
                if(process->readAll().isEmpty() && !file.fileName().contains(QRegularExpression(search_filter))){
                    files.removeAll(file);
                }
            }
        }

        //La liste de fichier n'est pas vide donc on procède à la création d'un bookmark
        if(!files.isEmpty()) {
            QSqlQuery q;
            //TODO: Pour le moment filter = search_name, on permettra à l'utilisateur de nommer sa recherche
            if(q.exec("INSERT INTO bookmark (filter, search_directory, search_name) VALUES ( \"" % search_filter %"\", \"" % curr_dir->absolutePath() % "\", \"" % search_filter % "\" )")) {
                QString lastBookmarkId = q.lastInsertId().toString();
                foreach (QFileInfo file, files) {
                    if(q.exec("INSERT INTO file (bookmark_id_fk, file_path) VALUES ( " % lastBookmarkId % ", \"" % file.filePath() % "\")")) {
                    } else {
                        QMessageBox::warning(this, "Erreur avec la BDD lors de la création d'un bookmark", "Medium n'a pas réussi à créer un file dans la base de donnée : \n\"" %
                                             q.lastError().driverText() %
                                             " : " %
                                             q.lastError().databaseText() %
                                             (q.lastError().nativeErrorCode().isEmpty() ? "" : " : " + q.lastError().nativeErrorCode()));
                        //break;
                    }
                }

            } else {
               QMessageBox::warning(this, "Erreur avec la BDD lors de la création d'un bookmark", "Medium n'a pas réussi à créer un bookmark dans la base de donnée : \n\"" %
                                    q.lastError().driverText() %
                                    " : " %
                                    q.lastError().databaseText() %
                                    (q.lastError().nativeErrorCode().isEmpty() ? "" : " : " + q.lastError().nativeErrorCode()));
            }
        } else {
            QMessageBox::information(this, "Aucun résultat", "Désolé mais le filtre choisi ne donne aucun résultat.");
        }

    } else {
        QMessageBox::information(this, "Recherche vide", "Veuillez saisir au moins un caractère pour effectuer une recherche.");
    }
    displayBookmarks();
}
