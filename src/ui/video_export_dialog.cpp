#include "include/ui/video_export_dialog.h"
#include "ui_video_export_dialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

VideoExportDialog::VideoExportDialog(QWidget *parent, const QFileInfoList &outputFiles) :
	QDialog(parent),
	ui(new Ui::VideoExportDialog),
    outputFiles(outputFiles)
{
	ui->setupUi(this);
    ui->edtFinalFrame->setText(QString::number(outputFiles.size()));
}

QProgressBar* VideoExportDialog::getProgressBar() const {
    return ui->progressBar;
}

void VideoExportDialog::on_btnBrowseOutputFile_clicked() {
    QString videoFile = QFileDialog::getSaveFileName(this, "Video Export", "", "Audio Video Interleave file (*.avi)");
    
    if (!videoFile.isEmpty()) {
        ui->edtOutputFile->setText(videoFile);
    }
}

void VideoExportDialog::accept() {
    int initialFrame = ui->edtInitialFrame->text().toInt();
    int finalFrame = ui->edtFinalFrame->text().toInt();
    
    if (initialFrame >= finalFrame) {
        QMessageBox::warning(this, "Video Export", "Invalid frame interval.");
        return;
    }
    
    int frameStep = ui->edtFrameStep->text().toInt();
    
    if (frameStep >= finalFrame - initialFrame) {
        QMessageBox::warning(this, "Video Export", QString("Frame step must be between %1 and %2.").arg(initialFrame).arg(finalFrame - 1));
        return;
    }
    
    int frameRate = ui->edtFrameRate->text().toInt();
    
    if (frameRate <= 0) {
        QMessageBox::warning(this, "Video Export", "Frame rate must be greater than 0.");
        return;
    }
    
    QString outputFile = ui->edtOutputFile->text();
    
    if (outputFile.isEmpty()) {
        QMessageBox::warning(this, "Video Export", "Invalid output file.");
        return;
    }
    
    ui->progressBar->setMinimum(initialFrame - 1);
    ui->progressBar->setMaximum(finalFrame);
    
    emit stopReproduction();
    emit exportVideo(initialFrame, finalFrame, frameStep, frameRate, outputFile);
    
    QDialog::accept();
}