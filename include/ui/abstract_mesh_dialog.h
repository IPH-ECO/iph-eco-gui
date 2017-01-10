#ifndef ABSTRACT_MESH_DIALOG_H
#define ABSTRACT_MESH_DIALOG_H

#include <ui/mesh_properties_dialog.h>

#include <QDialog>
#include <QWidget>
#include <QAction>
#include <QSettings>
#include <QShowEvent>

class AbstractMeshDialog : public QDialog {
	Q_OBJECT
protected:
    const QString GENERAL_DEFAULT_DIR_KEY;
    QSettings *appSettings;
	MeshVTKWidget *vtkWidget;
    QList<QAction*> toolBarActions;
    QAction *toggleVerticeLabelsAction;
    QAction *toggleCellLabelsAction;
    QAction *zoomAreaAction;
    QAction *changeBackgroundColorAction;
    bool enableMeshPropertiesAction;
    
    QString getDefaultDirectory();
public:
	explicit AbstractMeshDialog(QWidget *parent);
    virtual ~AbstractMeshDialog();

	virtual void showEvent(QShowEvent *event);
private slots:
	virtual void onToggleLabelsClicked(bool show);
	void onExportMapClicked();
	void onMeshPropertiesClicked();
    void onChangeBackgroundColorClicked();
};

#endif // ABSTRACT_MESH_DIALOG_H
