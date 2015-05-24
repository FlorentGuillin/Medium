#include "mainwindow.h"

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
    curr_dir = new QDir(QDir::homePath());
    ui->deleteBookmark_pushButton->setEnabled(false);
    displayDir(QDir::homePath());

    ftm = new FileTreeModel(QList<QStringList>());
    ui->file_treeView->setModel(ftm);
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
            BookmarkListWidgetItem *blwi = new BookmarkListWidgetItem(ui->bookmark_listWidget);
            blwi->setBookmarkId(q->value(0).toInt());
            blwi->setFilter(q->value(1).toString());
            blwi->setSearchDirectory(q->value(2).toString());
            blwi->setSearchName(q->value(3).toString());
            blwi->setIcon(bookmark_icon);
            blwi->setText(blwi->getSearchName());
        }
    } else {
        QMessageBox::warning(this, "Erreur de récupération des bookmarks", " Medium n'a pas réussi à récupérer les bookmarks : \n\"" %
                              q->lastError().driverText() %
                              " : " %
                              q->lastError().databaseText() %
                              (q->lastError().nativeErrorCode().isEmpty() ? "" : " : " + q->lastError().nativeErrorCode()));
    }
}

//Affiche les bookmarks de l'utilisateur
void MainWindow::displayFiles(int bookmark_id) {

    //Récupération des bookmarks
    QSqlQuery *q = new QSqlQuery(db);
    q->setForwardOnly(true);

    if(q->exec("SELECT * FROM file WHERE bookmark_id_fk = '" % QString::number(bookmark_id) % "'")) {

        QList<QStringList> files = QList<QStringList>();
        while(q->next()) {
            QStringList file = QStringList();
            file.append(q->value(0).toString());
            file.append(q->value(1).toString());
            file.append(q->value(2).toString());
            file.append(q->value(3).toString());
            files.append(file);
        }

        ftm->destroyed();
        ftm = new FileTreeModel(files);
        ui->file_treeView->setModel(ftm);
    } else {
        QMessageBox::warning(this, "Erreur de récupération des fichiers", " Medium n'a pas réussi à récupérer la liste des fichiers associés à votre bookmark : \n\"" %
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
            if((res = q.exec("CREATE TABLE IF NOT EXISTS file (file_id INTEGER PRIMARY KEY AUTOINCREMENT, bookmark_id_fk INTEGER, file_path TEXT, file_type TEXT, FOREIGN KEY(bookmark_id_fk) REFERENCES bookmark(bookmark_id))"))) {

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

//Renvoie la liste des dossiers ou fichiers qui peuvent être analysés par Medium en fonction du type MIME pour les fichiers
QFileInfoList MainWindow::getAllFilesRecursively(QDir* dir, QRegExp regexp_filter){
    QFileInfoList res = QFileInfoList();

    QStringList name_filter = QStringList("*");
    QFileInfoList files = dir->entryInfoList(name_filter, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    QMimeDatabase qmd;

    foreach (QFileInfo file, files){
        //Si l'élément courant est un dossier et contient un des termes de recherche, on l'ajoute à la liste (on suppose que le dossier et son contenu convient à l'utilisateur)
        //Exemple: si la recherche est "facture" et qu'il y a un dossier facture, on garde tout son contenu de manière récursive, si le choix n'est pas pertinent, l'utilisateur pourra de toute manière supprimer le dossier
        if (file.isDir() && file.fileName().contains(regexp_filter)){
            res.append(file);
        // L'élément est un dossier dont le nom n'a rien à voir avec la recherche, du coup, on ajoute tous les fichiers qu'il contient à la liste pour les tester individuellement par la suite
        } else if (file.isDir() && !file.fileName().contains(regexp_filter)) {
            res.append(getAllFilesRecursively(new QDir(file.filePath()), regexp_filter));
        }else{
            // On teste le type MIME des fichiers pour savoir si on peut traiter le fichier ou non avec nos algorithmes
            QString mimetype = qmd.mimeTypeForFile(file).name();
            if(mimetype.contains(QRegularExpression("(text/|image/|audio/)"))) {
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
        QRegExp regexp_filter = buildSearchRegExp(search_filter);
        if(regexp_filter.pattern() != "--Invalid QRegExp because filter--") {
            //Récupération de tous les fichiers contenu sous le répertoire courant
            QFileInfoList files = getAllFilesRecursively(curr_dir, regexp_filter);
            QMimeDatabase qmd;
            foreach(QFileInfo file, files) {
                if(!file.fileName().contains(regexp_filter)) {
                    //Récupération du MIME Type pour savoir si on traite ou pas le fichier
                    QString mimetype = qmd.mimeTypeForFile(file).name();
                    if(mimetype.contains(QRegularExpression("text/"))) {

                        bool isMatch = false;
                        QFile text_file(file.absoluteFilePath());
                        if (text_file.open(QIODevice::ReadOnly | QIODevice::Text))
                        {
                            QTextStream stream(&text_file);
                            while(!stream.atEnd()) {
                                QString line = stream.readLine();
                                if((isMatch = line.contains(regexp_filter))) {
                                    break;
                                } else {
                                }
                            }
                        }
                        text_file.close();

                        if(!isMatch) {
                            files.removeAll(file);
                        }

                    } else if(mimetype.contains(QRegularExpression("image/"))) {
                        ImageMetadata * image = new ImageMetadata(file.filePath());
                        if(!image->regexHasValue(regexp_filter)) {
                            files.removeAll(file);
                        }
                    } else if(mimetype.contains(QRegularExpression("audio/"))) {
                        AudioMetadata * audio = new AudioMetadata(file.filePath());
                        if(!audio->isValue(regexp_filter)) {
                            files.removeAll(file);
                        }

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
                        QString file_type = qmd.mimeTypeForFile(file).name();
                        if(q.exec("INSERT INTO file (bookmark_id_fk, file_path, file_type) VALUES ( " % lastBookmarkId % ", \"" % file.filePath() % "\", \"" % file_type % "\")")) {
                        } else {
                            QMessageBox::warning(this, "Erreur avec la BDD lors de la création d'un bookmark", "Medium n'a pas réussi à créer un file dans la base de donnée : \n\"" %
                                                 q.lastError().driverText() %
                                                 " : " %
                                                 q.lastError().databaseText() %
                                                 (q.lastError().nativeErrorCode().isEmpty() ? "" : " : " + q.lastError().nativeErrorCode()));
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
        }
    } else {
        QMessageBox::information(this, "Recherche vide", "Veuillez saisir au moins un caractère pour effectuer une recherche.");
    }
    displayBookmarks();
}

void MainWindow::on_bookmark_listWidget_clicked(const QModelIndex &index)
{
    BookmarkListWidgetItem *blwi = (BookmarkListWidgetItem *) ui->bookmark_listWidget->currentItem();
    ui->bookmarkSearchName_label->setText("Nom du bookmark : " % blwi->getSearchName());
    ui->bookmarkSearchDirectory_label->setToolTip(blwi->getSearchDirectory());
    ui->bookmarkSearchDirectory_label->setText("Répertoire : " % QDir(blwi->getSearchDirectory()).dirName());
    ui->bookmarkFilter_label->setText("Filtre de recherche : " % blwi->getFilter());
    ui->deleteBookmark_pushButton->setEnabled(true);
    displayFiles(blwi->getBookmarkId());
}

void MainWindow::on_deleteBookmark_pushButton_clicked()
{
    BookmarkListWidgetItem *blwi = (BookmarkListWidgetItem *) ui->bookmark_listWidget->currentItem();
    QSqlQuery *q = new QSqlQuery(db);
    if(q->exec("DELETE FROM file WHERE bookmark_id_fk = '" % QString::number(blwi->getBookmarkId()) % "'")) {
        if(q->exec("DELETE FROM bookmark WHERE bookmark_id = '" % QString::number(blwi->getBookmarkId()) % "'")) {
            displayBookmarks();
            ui->deleteBookmark_pushButton->setEnabled(false);
            ui->bookmarkSearchName_label->setText("Nom du bookmark : ");
            ui->bookmarkSearchDirectory_label->setToolTip("");
            ui->bookmarkSearchDirectory_label->setText("Répertoire : ");
            ui->bookmarkFilter_label->setText("Filtre de recherche : ");

            ftm->destroyed();
            ftm = new FileTreeModel(QList<QStringList>());
            ui->file_treeView->setModel(ftm);
        } else {
            QMessageBox::warning(this, "Erreur de suppression du bookmark", " Medium n'a pas réussi à supprimer votre bookmark : \n\"" %
                                  q->lastError().driverText() %
                                  " : " %
                                  q->lastError().databaseText() %
                                  (q->lastError().nativeErrorCode().isEmpty() ? "" : " : " + q->lastError().nativeErrorCode()));
        }
    } else {
        QMessageBox::warning(this, "Erreur de suppression des fichiers", " Medium n'a pas réussi à supprimer la liste des fichiers associés à votre bookmark : \n\"" %
                              q->lastError().driverText() %
                              " : " %
                              q->lastError().databaseText() %
                              (q->lastError().nativeErrorCode().isEmpty() ? "" : " : " + q->lastError().nativeErrorCode()));
    }
}

void MainWindow::on_file_treeView_clicked(const QModelIndex &index)
{
    /*QStandardItemModel *qsim = (QStandardItemModel *) ui->file_treeView->model();
    FileStandardItem *fsi =(FileStandardItem *) qsim->itemFromIndex(index);
    qDebug() << fsi->getFilePath();*/
    //ui->file_treeView->model()->itemFromIndex(index);
}

QRegExp MainWindow::buildSearchRegExp(QString search_filter){
    QString string_regexp = "";
    QString string_no_parentheses = "";
    bool filterIsValid = true;
    search_filter = search_filter.replace(" ", "");
    //Section de découpage des parenthèses
    int pos = -1;
    int openingPar = 0;
    int pos_last_good_couple = 0;
    for(int i = 0; i < search_filter.length(); i++) {
        if(search_filter.at(i) == '(' && openingPar == 0) { //On trouve une première parenthèse ouvrante (ou une nouvelle depuis un groupe bien parenthésée
            openingPar++;
            pos = i;
        } else if (search_filter.at(i) == '(' && openingPar > 0) { //On trouve une parenthèse ouvrante se trouvant après une autre parenthèse ouvrante qui n'est pas refermée
            openingPar++;
        } else if (search_filter.at(i) == ')' && openingPar == 1) { //On trouve une parenthèse fermante qui cloture un sous-groupe.
            openingPar--;
            if(pos_last_good_couple == 0) {
                string_no_parentheses.append(search_filter.mid(pos_last_good_couple, (pos-pos_last_good_couple)) + buildSearchRegExp(search_filter.mid(pos+1, (i-pos-1))).pattern());
            } else {
                string_no_parentheses.append(search_filter.mid((pos_last_good_couple+1), (pos-pos_last_good_couple-1)) + buildSearchRegExp(search_filter.mid(pos+1, (i-pos-1))).pattern());
            }
            pos_last_good_couple = i;
        } else if (search_filter.at(i) == ')' && openingPar > 1) { //On trouve une parenthèse fermante qui forme un couple compris dans un autre couple de parenthèse
            openingPar--;
        } else if (search_filter.at(i) == ')' && openingPar < 1) { //On trouve une parenthèse fermante sans parenthèse ouvrante, expression invalide
            filterIsValid = false;
            pos = i;
            break;
        }
    }

    if(pos == -1) {
        string_no_parentheses = search_filter;
    } else if(pos_last_good_couple != (search_filter.length() - 1)) {
        string_no_parentheses.append(search_filter.right((search_filter.length()- pos_last_good_couple - 1)));
    }

    if(filterIsValid) {
        //On coupe les AND
        QStringList string_no_and = string_no_parentheses.split("-AND-");
        if(string_no_and.count() > 1){
            //On analyse chaque opérande de AND
            foreach(QString and_op, string_no_and) {
                if(and_op != ""){
                    //On regarde s'il y a des OR dedans
                    QStringList string_no_or = and_op.split("-OR-");
                    //Il y en a un si la liste a plus de 1 élément
                    QString or_ok = "";
                    if(string_no_or.count() > 1) {
                        //On construit le bout de regexp associé
                        or_ok.append('(');
                        foreach(QString or_op, string_no_or) {
                            if(or_op != "") {
                                or_ok.append(or_op + "|");
                            }
                        }
                        //Suppresion du dernier pipe
                        or_ok = or_ok.left(or_ok.length()-1);
                        or_ok.append(')');
                    }
                    if(or_ok == "") {
                        string_regexp.append("(?=.*" + and_op + ")");
                    } else {
                        string_regexp.append("(?=.*" + or_ok + ")");
                    }
                }
            }
        } else {
            //On regarde s'il y a des OR dedans
            QStringList string_no_or = string_no_parentheses.split("-OR-");
            //Il y en a un si la liste a plus de 1 élément
            QString or_ok = "";
            if(string_no_or.count() > 1) {
                //On construit le bout de regexp associé
                or_ok.append('(');
                foreach(QString or_op, string_no_or) {
                    if(or_op != "") {
                        or_ok.append(or_op + "|");
                    }
                }
                //Suppresion du dernier pipe
                or_ok = or_ok.left(or_ok.length()-1);
                or_ok.append(')');
                string_regexp = or_ok;
            } else {
                string_regexp = search_filter;
            }
        }
        QRegExp regexp_filter = QRegExp(string_regexp);
        regexp_filter.setCaseSensitivity(Qt::CaseInsensitive);
        return regexp_filter;
    } else {
        QMessageBox::warning(this, "Erreur dans le parenthésage de votre filtre", " ')' (pos : " % QString::number(pos) % ") dans le filtre n'est associé à aucune ')'.");
        return QRegExp("--Invalid QRegExp because filter--");
    }
}
