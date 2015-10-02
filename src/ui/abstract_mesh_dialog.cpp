#include "include/ui/abstract_mesh_dialog.h"

#include "include/ui/main_window.h"

#include <QIcon>
#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>

AbstractMeshDialog::AbstractMeshDialog(QWidget *parent) : QDialog(parent), BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"), vtkWidget(nullptr) {
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

AbstractMeshDialog::~AbstractMeshDialog() {
    delete appSettings;
}

void AbstractMeshDialog::showEvent(QShowEvent *event) {
	if (!event->spontaneous() && this->windowState() & Qt::WindowMaximized) {
        MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
        
        QAction *separator = mainWindow->getToolBar()->addSeparator();
        QAction *zoomOriginalAction = new QAction(QIcon(":/icons/zoom-original.png"), "Reset zoom", mainWindow);

        zoomAreaAction = new QAction(QIcon(":/icons/zoom-select.png"), "Zoom area", mainWindow);
        zoomAreaAction->setCheckable(true);
        
//        QAction *lockViewAction = new QAction(QIcon(":/icons/lock-view.png"), "Lock/Unlock view", mainWindow);
//        lockViewAction->setChecked(true);
        
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
        
        QAction *meshPropertiesAction = new QAction(QIcon(":/icons/format-list-unordered.png"), "Change mesh properties", mainWindow);
        QAction *exportMapAction = new QAction(QIcon(":/icons/image-x-generic.png"), "Export map to PNG", mainWindow);
        
        toolBarActions.append(separator);
        toolBarActions.append(zoomOriginalAction);
        toolBarActions.append(zoomAreaAction);
//        toolBarActions.append(lockViewAction);
        toolBarActions.append(toggleAxesAction);
        toolBarActions.append(toggleMeshAction);
        toolBarActions.append(toggleBoundaryEdgesAction);
        toolBarActions.append(toggleCellLabelsAction);
        toolBarActions.append(toggleVerticeLabelsAction);
        toolBarActions.append(exportMapAction);
        toolBarActions.append(meshPropertiesAction);
        
        QObject::connect(zoomAreaAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleZoomArea(bool)));
        QObject::connect(zoomOriginalAction, SIGNAL(triggered()), vtkWidget, SLOT(resetZoom()));
        QObject::connect(toggleAxesAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleAxes(bool)));
        QObject::connect(toggleMeshAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleMesh(bool)));
        QObject::connect(toggleBoundaryEdgesAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleBoundaryEdges(bool)));
        QObject::connect(toggleCellLabelsAction, SIGNAL(triggered(bool)), this, SLOT(onToggleLabelsClicked(bool)));
        QObject::connect(toggleVerticeLabelsAction, SIGNAL(triggered(bool)), this, SLOT(onToggleLabelsClicked(bool)));
        QObject::connect(exportMapAction, SIGNAL(triggered()), this, SLOT(onExportMapClicked()));
        QObject::connect(meshPropertiesAction, SIGNAL(triggered()), this, SLOT(onMeshPropertiesClicked()));
        
        mainWindow->getToolBar()->addAction(zoomOriginalAction);
        mainWindow->getToolBar()->addAction(zoomAreaAction);
        mainWindow->getToolBar()->addAction(toggleMeshAction);
        mainWindow->getToolBar()->addAction(toggleBoundaryEdgesAction);
        mainWindow->getToolBar()->addAction(toggleAxesAction);
        mainWindow->getToolBar()->addAction(toggleCellLabelsAction);
        mainWindow->getToolBar()->addAction(toggleVerticeLabelsAction);
        mainWindow->getToolBar()->addAction(meshPropertiesAction);
        mainWindow->getToolBar()->addAction(exportMapAction);
    }
    
    QDialog::showEvent(event);
}

void AbstractMeshDialog::onToggleLabelsClicked(bool show) {
    QAction *sender = static_cast<QAction*>(QObject::sender());
    bool toggleVerticeLabels = sender == toggleVerticeLabelsAction;
    LabelType labelType = toggleVerticeLabels ? LabelType::VERTICE_ID : LabelType::CELL_ID;
    
    if (toggleVerticeLabels && show && toggleCellLabelsAction->isChecked()) {
        toggleCellLabelsAction->setChecked(false);
    } else if (!toggleVerticeLabels && show && toggleVerticeLabelsAction->isChecked()) {
        toggleVerticeLabelsAction->setChecked(false);
    }
    
    vtkWidget->toggleLabels(show ? labelType : LabelType::UNDEFINED);
}

void AbstractMeshDialog::onExportMapClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to PNG"), getDefaultDirectory(), tr("Portable Network Graphics (*.png)"));
    
    if (!fileName.isEmpty()) {
        vtkWidget->exportToImage(fileName);
        QMessageBox::information(this, tr("Structured Mesh Generation"), tr("Map exported."));
    }
}

void AbstractMeshDialog::onMeshPropertiesClicked() {
    if (vtkWidget->getMesh()) {
        MeshPropertiesDialog *dialog = new MeshPropertiesDialog(this, vtkWidget);
        dialog->exec();
    }
}

QString AbstractMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}