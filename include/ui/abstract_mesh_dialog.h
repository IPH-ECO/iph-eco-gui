#ifndef ABSTRACT_MESH_DIALOG_H
#define ABSTRACT_MESH_DIALOG_H

#include "include/ui/mesh_properties_dialog.h"

#include <QDialog>
#include <QWidget>
#include <QAction>
#include <QSettings>
#include <QShowEvent>

class AbstractMeshDialog : public QDialog {
	Q_OBJECT
protected:
    const QString BOUNDARY_DEFAULT_DIR_KEY;
    QSettings *appSettings;
	MeshVTKWidget *vtkWidget;
    QList<QAction*> toolBarActions;
    QAction *toggleVerticeLabelsAction;
    QAction *toggleCellLabelsAction;
    
    QString getDefaultDirectory();
public:
	explicit AbstractMeshDialog(QWidget *parent);
    virtual ~AbstractMeshDialog();

	virtual void showEvent(QShowEvent *event);

private slots:
	void onToggleLabelsClicked(bool show);
	void onExportMapClicked();
	void onMeshPropertiesClicked();
};

#endif // ABSTRACT_MESH_DIALOG_H