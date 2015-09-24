#ifndef METEROLOGICAL_STATION_H
#define METEROLOGICAL_STATION_H

#include "meteorological_parameter.h"

#include <QMetaType>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkCaptionActor2D.h>

class MeteorologicalStation {
private:
    uint id;
    QString name;
    bool useLatitudeLongitude;
    double utmX;
    double utmY;
    double latitude;
    double longitude;
    QList<MeteorologicalParameter*> parameters;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> iconActor;
    vtkSmartPointer<vtkCaptionActor2D> captionActor;
public:
    MeteorologicalStation();
    MeteorologicalStation(const QString &name);
    uint getId() const;
    void setId(uint id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    bool getUseLatitudeLongitude() const;
    void setUseLatitudeLongitude(bool useLatitudeLongitude);
    double getUtmX() const;
    void setUtmX(double utmX);
    double getUtmY() const;
    void setUtmY(double utmY);
    double getLatitude() const;
    void setLatitude(double latitude);
    double getLongitude() const;
    void setLongitude(double longitude);
    QList<MeteorologicalParameter*> getParameters() const;
    void setParameters(const QList<MeteorologicalParameter*> &parameters);
    void addMeteorologicalParameter(MeteorologicalParameter *parameter);
    vtkSmartPointer<vtkActor> getIconActor() const;
    void setIconActor(vtkSmartPointer<vtkActor> iconActor);
    vtkSmartPointer<vtkCaptionActor2D> getCaptionActor() const;
    void setCaptionActor(vtkSmartPointer<vtkCaptionActor2D> captionActor);
};

Q_DECLARE_METATYPE(MeteorologicalStation*)

#endif // METEROLOGICAL_STATION_H
