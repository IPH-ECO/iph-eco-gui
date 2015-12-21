#ifndef ABSTRACT_MESH_DIALOG_H
#define ABSTRACT_MESH_DIALOG_H

#include <ui/mesh_properties_dialog.h>

#include <QDialog>
#include <QWidget>
#include <QAction>
#include <QSettings>
#include <QShowEvent>

enum class AbstractDialogAction {
    RESET_ZOOM = (1 << 0),
    AREA_ZOOM = (1 << 1),
    MESH_VISIBILITY = (1 << 2),
    AXES_VISIBILITY = (1 << 3),
    CELL_IDS_VISIBILITY = (1 << 4),
    VERTICE_IDS_VISIBILITY = (1 << 5),
    MESH_PROPERTIES = (1 << 6),
    EXPORT_AS_PICTURE = (1 << 7),
    EXPORT_AS_VIDEO = (1 << 8),
    CHANGE_BACKGROUND_COLOR = (1 << 9)
};

class AbstractMeshDialog : public QDialog {
	Q_OBJECT
protected:
    const QString BOUNDARY_DEFAULT_DIR_KEY;
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
protected slots:
    virtual void closeDialog();
private slots:
	virtual void onToggleLabelsClicked(bool show);
	void onExportMapClicked();
	void onMeshPropertiesClicked();
    void onChangeBackgroundColorClicked();
};

#endif // ABSTRACT_MESH_DIALOG_H