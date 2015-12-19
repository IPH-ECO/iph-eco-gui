#ifndef VIDEO_EXPORT_DIALOG_H
#define VIDEO_EXPORT_DIALOG_H

#include "include/domain/simulation.h"

#include <QWidget>
#include <QDialog>
#include <QProgressBar>
#include <QFileInfoList>

namespace Ui {
	class VideoExportDialog;
}

class VideoExportDialog : public QDialog {
	Q_OBJECT
private:
	Ui::VideoExportDialog *ui;
    QFileInfoList outputFiles;
    
    virtual void accept();
public:
	explicit VideoExportDialog(QWidget *parent, const QFileInfoList &outputFiles);
    QProgressBar* getProgressBar() const;
private slots:
    void on_btnBrowseOutputFile_clicked();
signals:
    void stopReproduction();
    void exportVideo(int initialFrame, int finalFrame, int frameStep, int frameRate, const QString &outputFile);
};

#endif // VIDEO_EXPORT_DIALOG_H