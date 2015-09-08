#include "include/ui/structured_mesh_dialog.h"
#include "ui_structured_mesh_dialog.h"
#include "include/ui/main_window.h"
#include "include/application/iph_application.h"
#include "include/exceptions/mesh_polygon_exception.h"
#include "include/ui/island_form.h"

#include <QProgressDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>

StructuredMeshDialog::StructuredMeshDialog(QWidget *parent) :
    QDialog(parent), BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"), ui(new Ui::StructuredMeshDialog), unsavedMesh(new StructuredMesh), currentMesh(unsavedMesh)
{
    ui->setupUi(this);
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    ui->cbxMeshName->blockSignals(true);
    for (Mesh *mesh : meshes) {
        if (mesh->instanceOf("StructuredMesh")) {
            ui->cbxMeshName->addItem(mesh->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    
    connect(ui->lstIslands->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(on_islandItemEdited(const QModelIndex&, const QModelIndex&)));
}

StructuredMeshDialog::~StructuredMeshDialog() {
    delete appSettings;
    delete unsavedMesh;
    delete ui;
}

QString StructuredMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}

void StructuredMeshDialog::setCoordinate(double &x, double &y) {
    QString xStr = QString::number(x, 'f', 6);
    QString yStr = QString::number(y, 'f', 6);

    ui->lblUTMCoordinate->setText(QString("Easting: %1, Northing: %2").arg(xStr).arg(yStr));
}

void StructuredMeshDialog::setArea(const double &area) {
	QString areaStr("Area: ");

	if (area == 0.0) {
		areaStr += "-";
	} else {
		areaStr += QString::number(area, 'f', 2) + " m\u00B2";
	}

	ui->lblDomainArea->setText(areaStr);
}

void StructuredMeshDialog::on_cbxMeshName_currentTextChanged(const QString &meshName) {
    if (meshName.isEmpty()) {
        return;
    }
    
    currentMesh = static_cast<StructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(meshName));
    MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
    QList<MeshPolygon*> islands = currentMesh->getIslands();
    
    ui->edtMeshName->setText(currentMesh->getName());
    ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
    ui->sbxResolution->setValue(currentMesh->getResolution());
    ui->lstIslands->clear();
    
    for (MeshPolygon *island : islands) {
        QListWidgetItem *item = new QListWidgetItem(island->getName());
        
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->lstIslands->addItem(item);
    }
    
    ui->vtkWidget->render(currentMesh);
}

void StructuredMeshDialog::on_btnNewMesh_clicked() {
    unsavedMesh->clear();
    currentMesh = unsavedMesh;
    ui->cbxMeshName->blockSignals(true);
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    ui->edtMeshName->setText("");
    ui->edtMeshName->setFocus();
    ui->edtBoundaryFileLine->setText("");
    ui->sbxResolution->setValue(100);
    ui->lstIslands->clear();
    ui->vtkWidget->clear();
    // TODO: update status bar text
}

void StructuredMeshDialog::on_btnApplyMesh_clicked() {
    QString oldMeshName = ui->cbxMeshName->currentText();
    QString newMeshName = ui->edtMeshName->text();
    
    if (newMeshName.isEmpty()) {
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Mesh name can't be empty."));
        return;
    }
    
    if (oldMeshName.isEmpty()) { // new mesh
        Project *project = IPHApplication::getCurrentProject();
        
        if (project->containsMesh(newMeshName)) {
            QMessageBox::critical(this, tr("Structured Mesh Generation"), tr("Mesh name already used."));
            return;
        }
        
        currentMesh->setName(newMeshName);
        project->addMesh(currentMesh);
        unsavedMesh = new StructuredMesh();
        
        ui->cbxMeshName->addItem(newMeshName);
        ui->cbxMeshName->setCurrentText(newMeshName);
    } else {
        currentMesh->setName(newMeshName);
        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), newMeshName);
    }
}

void StructuredMeshDialog::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();
    
    if (meshName.isEmpty()) {
        return;
    }
    
    QString questionStr = tr("Are you sure you want to remove '") + meshName + "'? Any existing associations with this mesh will be also removed.";
    QMessageBox::StandardButton questionBtn = QMessageBox::question(this, tr("Remove mesh"), questionStr);
    
    if (questionBtn == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeMesh(currentMesh);
        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        this->on_btnNewMesh_clicked();
    }
}

void StructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (boundaryFilePath.isEmpty()) {
        return;
    }

    ui->edtBoundaryFileLine->setText(boundaryFilePath);
    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
}

void StructuredMeshDialog::on_btnAddIsland_clicked() {
    IslandForm *islandForm = new IslandForm(this, currentMesh);
    int exitCode = islandForm->exec();
    
    if (exitCode == QDialog::Accepted) {
        MeshPolygon *meshPolygon = islandForm->getMeshPolygon();
        QListWidgetItem *item = new QListWidgetItem(meshPolygon->getName());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->lstIslands->addItem(item);
    }
    
    delete islandForm;
}

void StructuredMeshDialog::on_btnRemoveIsland_clicked() {
    if (!currentMeshPolygonName.isEmpty()) {
        QString questionStr = tr("Are you sure you want to remove the selected island?");
        QMessageBox::StandardButton questionBtn = QMessageBox::question(this, tr("Structured Mesh Generation"), questionStr);

        if (questionBtn == QMessageBox::Yes) {
            currentMesh->removeMeshPolygon(currentMeshPolygonName, MeshPolygonType::ISLAND);
            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
        }
    }
}

void StructuredMeshDialog::on_btnGenerateMesh_clicked() {
    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    
    if (!boundaryFileStr.isEmpty() || !currentMesh->isPersisted()) {
        QFile boundaryFile(boundaryFileStr);
        QFileInfo boundaryFileInfo(boundaryFile);

        if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
            QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Boundary file not found."));
            return;
        }
    }
    
    try {
        currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY);
    } catch (const MeshPolygonException& e) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), e.what());
        return;
    }
    
    ulong bounds[4];
    
    currentMesh->setResolution(ui->sbxResolution->value());
    currentMesh->computeBounds(bounds);
    
    int columns = (bounds[1] - bounds[0]) / currentMesh->getResolution(); // xmax - xmin
    int rows = (bounds[3] - bounds[2]) / currentMesh->getResolution(); // ymax - ymin
    QProgressDialog *progressDialog = new QProgressDialog(tr("Generating mesh..."), tr("Cancel"), 0, columns * rows - 1, this);
    QObject::connect(currentMesh, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
    QObject::connect(progressDialog, SIGNAL(canceled()), currentMesh, SLOT(cancelGeneration()));
    progressDialog->setMinimumDuration(500);
    progressDialog->setWindowModality(Qt::WindowModal);
    
    currentMesh->generate();
    
    if (progressDialog->wasCanceled()) {
        currentMesh->cancelGeneration(false); // Set false for future computations
    } else {
        ui->vtkWidget->render(currentMesh);
    }
    
    delete progressDialog;
}

void StructuredMeshDialog::on_lstIslands_currentTextChanged(const QString &currentText) {
    currentMeshPolygonName = currentText;
}

void StructuredMeshDialog::on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    MeshPolygon *meshPolygon = currentMesh->getMeshPolygon(currentMeshPolygonName, MeshPolygonType::ISLAND);
    meshPolygon->setName(topLeft.data().toString());
}

void StructuredMeshDialog::on_btnClose_clicked() {
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    
    for (QAction *action : toolBarActions) {
        mainWindow->getToolBar()->removeAction(action);
    }
    
    QMdiSubWindow *parentWindow = static_cast<QMdiSubWindow*>(parent());
    parentWindow->close();
}

void StructuredMeshDialog::showEvent(QShowEvent *event) {
    if (!event->spontaneous() && this->windowState() & Qt::WindowMaximized) {
        MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
        
        QAction *separator = mainWindow->getToolBar()->addSeparator();
        QAction *zoomOriginalAction = new QAction(QIcon(":/icons/zoom-original.png"), "Reset zoom", mainWindow);

        QAction *zoomAreaAction = new QAction(QIcon(":/icons/zoom-select.png"), "Zoom area", mainWindow);
        zoomAreaAction->setCheckable(true);
        
        QAction *toggleMeshAction = new QAction(QIcon(":/icons/unstructured-mesh.png"), "Show/Hide mesh", mainWindow);
        toggleMeshAction->setCheckable(true);
        toggleMeshAction->setChecked(true);
        
        QAction *toggleBoundaryEdgesAction = new QAction(QIcon(":/icons/boundary-domain.png"), "Show/Hide boundary edges", mainWindow);
        toggleBoundaryEdgesAction->setCheckable(true);
        toggleBoundaryEdgesAction->setChecked(true);
        
        QAction *toggleAxesAction = new QAction(QIcon(":/icons/show-axis.png"), "Show/Hide axes", mainWindow);
        toggleAxesAction->setCheckable(true);
        toggleAxesAction->setChecked(true);
        
        toggleCellLabelsAction = new QAction(QIcon(":/icons/show-cell-labels-mesh.png"), "Show/Hide cell ids", mainWindow);
        toggleCellLabelsAction->setCheckable(true);
        
        toggleVerticeLabelsAction = new QAction(QIcon(":/icons/show-vertice-labels.png"), "Show/Hide cell ids", mainWindow);
        toggleVerticeLabelsAction->setCheckable(true);
        
        toolBarActions.append(separator);
        toolBarActions.append(zoomOriginalAction);
        toolBarActions.append(zoomAreaAction);
        toolBarActions.append(toggleAxesAction);
        toolBarActions.append(toggleMeshAction);
        toolBarActions.append(toggleBoundaryEdgesAction);
        toolBarActions.append(toggleCellLabelsAction);
        toolBarActions.append(toggleVerticeLabelsAction);
        
        QObject::connect(zoomAreaAction, SIGNAL(triggered(bool)), ui->vtkWidget, SLOT(toggleZoomArea(bool)));
        QObject::connect(zoomOriginalAction, SIGNAL(triggered()), ui->vtkWidget, SLOT(resetZoom()));
        QObject::connect(toggleAxesAction, SIGNAL(triggered(bool)), ui->vtkWidget, SLOT(toggleAxes(bool)));
        QObject::connect(toggleMeshAction, SIGNAL(triggered(bool)), ui->vtkWidget, SLOT(toggleMesh(bool)));
        QObject::connect(toggleBoundaryEdgesAction, SIGNAL(triggered(bool)), ui->vtkWidget, SLOT(toggleBoundaryEdges(bool)));
        QObject::connect(toggleCellLabelsAction, SIGNAL(triggered(bool)), this, SLOT(onToggleLabelsClicked(bool)));
        QObject::connect(toggleVerticeLabelsAction, SIGNAL(triggered(bool)), this, SLOT(onToggleLabelsClicked(bool)));
        
        mainWindow->getToolBar()->addAction(zoomOriginalAction);
        mainWindow->getToolBar()->addAction(zoomAreaAction);
        mainWindow->getToolBar()->addAction(toggleMeshAction);
        mainWindow->getToolBar()->addAction(toggleBoundaryEdgesAction);
        mainWindow->getToolBar()->addAction(toggleAxesAction);
        mainWindow->getToolBar()->addAction(toggleCellLabelsAction);
        mainWindow->getToolBar()->addAction(toggleVerticeLabelsAction);
    }
    
    QDialog::showEvent(event);
}

void StructuredMeshDialog::onToggleLabelsClicked(bool show) {
    QAction *sender = static_cast<QAction*>(QObject::sender());
    bool toggleVerticeLabels = sender == toggleVerticeLabelsAction;
    LabelType labelType = toggleVerticeLabels ? LabelType::VERTICE_ID : LabelType::CELL_ID;
    
    if (toggleVerticeLabels && show && toggleCellLabelsAction->isChecked()) {
        toggleCellLabelsAction->setChecked(false);
    } else if (!toggleVerticeLabels && show && toggleVerticeLabelsAction->isChecked()) {
        toggleVerticeLabelsAction->setChecked(false);
    }
    
    ui->vtkWidget->toggleLabels(show ? labelType : LabelType::UNDEFINED);
}