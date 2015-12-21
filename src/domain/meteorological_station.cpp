#include <domain/meteorological_station.h>

MeteorologicalStation::MeteorologicalStation() : id(0) {}

MeteorologicalStation::MeteorologicalStation(const QString &name) : id(0), name(name), useLatitudeLongitude(false), latitude(0), longitude(0) {}

uint MeteorologicalStation::getId() const {
    return id;
}

void MeteorologicalStation::setId(uint id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

bool MeteorologicalStation::isPersisted() const {
    return id != 0;
}

QString MeteorologicalStation::getName() const {
    return name;
}

void MeteorologicalStation::setName(const QString &name) {
    this->name = name;
}

bool MeteorologicalStation::getUseLatitudeLongitude() const {
    return useLatitudeLongitude;
}

void MeteorologicalStation::setUseLatitudeLongitude(bool useLatitudeLongitude) {
    this->useLatitudeLongitude = useLatitudeLongitude;
}

double MeteorologicalStation::getUtmX() const {
    return utmX;
}

void MeteorologicalStation::setUtmX(double utmX) {
    this->utmX = utmX;
}

double MeteorologicalStation::getUtmY() const {
    return utmY;
}

void MeteorologicalStation::setUtmY(double utmY) {
    this->utmY = utmY;
}

double MeteorologicalStation::getLatitude() const {
    return latitude;
}

void MeteorologicalStation::setLatitude(double latitude) {
    this->latitude = latitude;
}

double MeteorologicalStation::getLongitude() const {
    return longitude;
}
void MeteorologicalStation::setLongitude(double longitude) {
    this->longitude = longitude;
}

QList<MeteorologicalParameter*> MeteorologicalStation::getParameters() const {
    return parameters;
}

void MeteorologicalStation::setParameters(const QList<MeteorologicalParameter*> &parameters) {
    this->parameters = parameters;
}

void MeteorologicalStation::addMeteorologicalParameter(MeteorologicalParameter *parameter) {
    parameters.append(parameter);
}

vtkSmartPointer<vtkActor> MeteorologicalStation::getIconActor() const {
    return iconActor;
}

void MeteorologicalStation::setIconActor(vtkSmartPointer<vtkActor> iconActor) {
    this->iconActor = iconActor;
}

vtkSmartPointer<vtkCaptionActor2D> MeteorologicalStation::getCaptionActor() const {
    return captionActor;
}

void MeteorologicalStation::setCaptionActor(vtkSmartPointer<vtkCaptionActor2D> captionActor) {
    this->captionActor = captionActor;
}

SimulationDataType::MeteorologicalStation MeteorologicalStation::toSimulationDataType() const {
    SimulationDataType::MeteorologicalStation station;
    
    station.utmX = this->utmX;
    station.utmY = this->utmY;
    station.parametersLength = this->parameters.size();
    station.parameters = new SimulationDataType::MeteorologicalParameter[this->parameters.size()];
    
    for (int i = 0; i < this->parameters.size(); i++) {
        station.parameters[i] = this->parameters[i]->toSimulationDataType();
    }
    
    return station;
}