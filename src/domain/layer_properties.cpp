#include <domain/layer_properties.h>

LayerProperties::LayerProperties() :
    useCustomMapMinimum(false),
    useCustomMapMaximum(false),
    customMapMinimumRange(0),
    customMapMaximumRange(0),
    mapColorGradient(ColorGradientTemplate::defaultTemplateName),
    mapInvertColorGradient(true),
    mapOpacity(100),
    mapLegend(true),
    mapLighting(false),
    useCustomPointsMinimum(false),
    useCustomPointsMaximum(false),
    customPointsMinimumRange(0),
    customPointsMaximumRange(0),
    pointsColorGradient(ColorGradientTemplate::defaultTemplateName),
    pointsInvertColorGradient(false),
    pointsOpacity(100),
    pointsSize(1),
    pointsLegend(false),
    useCustomVectorsMinimum(false),
    useCustomVectorsMaximum(false),
    customVectorsMinimumRange(0),
    customVectorsMaximumRange(0),
    vectorColorMode(VectorColorMode::MAGNITUDE),
    vectorsColorGradient(ColorGradientTemplate::defaultTemplateName),
    vectorsColor("#000000"),
    vectorsInvertColorGradient(true),
    vectorsOpacity(100),
    vectorsScale(0.25),
    vectorsWidth(1),
    vectorsLegend(true),
    meshLineColor("#000000"),
    meshLineStyle(0xFFFF),
    meshLineWidth(1),
    meshOpacity(100)
{}

double LayerProperties::getCustomMapMininumRange() const {
    return customMapMinimumRange;
}

void LayerProperties::setCustomMapMinimumRange(const double &customMapMinimumRange) {
    this->customMapMinimumRange = customMapMinimumRange;
}

double LayerProperties::getCustomMapMaximumRange() const {
    return customMapMaximumRange;
}

void LayerProperties::setCustomMapMaximumRange(const double &customMapMaximumRange) {
    this->customMapMaximumRange = customMapMaximumRange;
}

QString LayerProperties::getMapColorGradient() const {
    return mapColorGradient;
}

void LayerProperties::setMapColorGradient(const QString &mapColorGradient) {
    this->mapColorGradient = mapColorGradient;
}

bool LayerProperties::getMapInvertColorGradient() const {
    return mapInvertColorGradient;
}

void LayerProperties::setMapInvertColorGradient(const bool &mapInvertColorGradient) {
    this->mapInvertColorGradient = mapInvertColorGradient;
}

int LayerProperties::getMapOpacity() const {
    return mapOpacity;
}

void LayerProperties::setMapOpacity(const int &mapOpacity) {
    this->mapOpacity = mapOpacity;
}

bool LayerProperties::getMapLegend() const {
    return mapLegend;
}

void LayerProperties::setMapLegend(const bool &mapLegend) {
    this->mapLegend = mapLegend;
}

bool LayerProperties::getMapLighting() const {
    return mapLighting;
}

void LayerProperties::setMapLighting(const bool &mapLighting) {
    this->mapLighting = mapLighting;
}

double LayerProperties::getCustomPointsMininumRange() const {
    return customPointsMinimumRange;
}

void LayerProperties::setCustomPointsMinimumRange(const double &customPointsMinimumRange) {
    this->customPointsMinimumRange = customPointsMinimumRange;
}

double LayerProperties::getCustomPointsMaximumRange() const {
    return customPointsMaximumRange;
}

void LayerProperties::setCustomPointsMaximumRange(const double &customPointsMaximumRange) {
    this->customPointsMaximumRange = customPointsMaximumRange;
}

QString LayerProperties::getPointsColorGradient() const {
    return pointsColorGradient;
}

void LayerProperties::setPointsColorGradient(const QString &pointsColorGradient) {
    this->pointsColorGradient = pointsColorGradient;
}

bool LayerProperties::getPointsInvertColorGradient() const {
    return pointsInvertColorGradient;
}

void LayerProperties::setPointsInvertColorGradient(const bool &pointsInvertColorGradient) {
    this->pointsInvertColorGradient = pointsInvertColorGradient;
}

int LayerProperties::getPointsOpacity() const {
    return pointsOpacity;
}

void LayerProperties::setPointsOpacity(const int &pointsOpacity) {
    this->pointsOpacity = pointsOpacity;
}

int LayerProperties::getPointsSize() const {
    return pointsSize;
}

void LayerProperties::setPointsSize(const int &pointsSize) {
    this->pointsSize = pointsSize;
}

bool LayerProperties::getPointsLegend() const {
    return pointsLegend;
}

void LayerProperties::setPointsLegend(const bool &pointsLegend) {
    this->pointsLegend = pointsLegend;
}

double LayerProperties::getCustomVectorsMinimumRange() const {
    return customVectorsMinimumRange;
}

void LayerProperties::setCustomVectorsMinimumRange(const double &customVectorsMinimumRange) {
    this->customVectorsMinimumRange = customVectorsMinimumRange;
}

double LayerProperties::getCustomVectorsMaximumRange() const {
    return customVectorsMaximumRange;
}

void LayerProperties::setCustomVectorsMaximumRange(const double &customVectorsMaximumRange) {
    this->customVectorsMaximumRange = customVectorsMaximumRange;
}

bool LayerProperties::getUseCustomVectorsMinimum() const {
    return useCustomVectorsMinimum;
}

void LayerProperties::setUseCustomVectorsMinimum(const bool &useCustomVectorsMinimum) {
    this->useCustomVectorsMinimum = useCustomVectorsMinimum;
}

bool LayerProperties::getUseCustomVectorsMaximum() const {
    return useCustomVectorsMaximum;
}

void LayerProperties::setUseCustomVectorsMaximum(const bool &useCustomVectorsMaximum) {
    this->useCustomVectorsMaximum = useCustomVectorsMaximum;
}

VectorColorMode LayerProperties::getVectorColorMode() const {
    return vectorColorMode;
}

void LayerProperties::setVectorColorMode(const VectorColorMode &vectorColorMode) {
    this->vectorColorMode = vectorColorMode;
}

QString LayerProperties::getVectorsColorGradient() const {
    return vectorsColorGradient;
}

void LayerProperties::setVectorsColorGradient(const QString &vectorsColorGradient) {
    this->vectorsColorGradient = vectorsColorGradient;
}

QString LayerProperties::getVectorsColor() const {
    return vectorsColor;
}

void LayerProperties::setVectorsColor(const QString &vectorsColor) {
    this->vectorsColor = vectorsColor;
}

bool LayerProperties::getVectorsInvertColorGradient() const {
    return vectorsInvertColorGradient;
}

void LayerProperties::setVectorsInvertColorGradient(const bool &vectorsInvertColorGradient) {
    this->vectorsInvertColorGradient = vectorsInvertColorGradient;
}

int LayerProperties::getVectorsOpacity() const {
    return vectorsOpacity;
}

void LayerProperties::setVectorsOpacity(const int &vectorsOpacity) {
    this->vectorsOpacity = vectorsOpacity;
}

double LayerProperties::getVectorsScale() const {
    return vectorsScale;
}

void LayerProperties::setVectorsScale(const double &vectorsScale) {
    this->vectorsScale = vectorsScale;
}

int LayerProperties::getVectorsWidth() const {
    return vectorsWidth;
}

void LayerProperties::setVectorsWidth(const int &vectorsWidth) {
    this->vectorsWidth = vectorsWidth;
}

bool LayerProperties::getVectorsLegend() const {
    return vectorsLegend;
}

void LayerProperties::setVectorsLegend(const bool &vectorsLegend) {
    this->vectorsLegend = vectorsLegend;
}

QString LayerProperties::getMeshLineColor() const {
    return meshLineColor;
}

void LayerProperties::setMeshLineColor(const QString &meshLineColor) {
    this->meshLineColor = meshLineColor;
}

int LayerProperties::getMeshLineStyle() const {
    return meshLineStyle;
}

void LayerProperties::setMeshLineStyle(const int &meshLineStyle) {
    this->meshLineStyle = meshLineStyle;
}

int LayerProperties::getMeshLineWidth() const {
    return meshLineWidth;
}

void LayerProperties::setMeshLineWidth(const int &meshLineWidth) {
    this->meshLineWidth = meshLineWidth;
}

int LayerProperties::getMeshOpacity() const {
    return meshOpacity;
}

void LayerProperties::setMeshOpacity(const int &meshOpacity) {
    this->meshOpacity = meshOpacity;
}

bool LayerProperties::getUseCustomMapMinimum() const {
    return useCustomMapMinimum;
}

void LayerProperties::setUseCustomMapMinimum(const bool &useCustomMapMinimum) {
    this->useCustomMapMinimum = useCustomMapMinimum;
}

bool LayerProperties::getUseCustomMapMaximum() const {
    return useCustomMapMaximum;
}

void LayerProperties::setUseCustomMapMaximum(const bool &useCustomMapMaximum) {
    this->useCustomMapMaximum = useCustomMapMaximum;
}

bool LayerProperties::getUseCustomPointsMinimum() const {
    return useCustomPointsMinimum;
}

void LayerProperties::setUseCustomPointsMinimum(const bool &useCustomPointsMinimum) {
    this->useCustomPointsMinimum = useCustomPointsMinimum;
}

bool LayerProperties::getUseCustomPointsMaximum() const {
    return useCustomPointsMaximum;
}

void LayerProperties::setUseCustomPointsMaximum(const bool &useCustomPointsMaximum) {
    this->useCustomPointsMaximum = useCustomPointsMaximum;
}