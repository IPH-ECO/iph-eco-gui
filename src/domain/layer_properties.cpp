#include "include/domain/layer_properties.h"

LayerProperties::LayerProperties() :
    mapMinimumRange(0),
    mapMaximumRange(0),
    mapColorGradient(ColorGradientTemplate::defaultTemplateName),
    mapInvertColorGradient(false),
    mapOpacity(100),
    mapLegend(true),
    mapLighting(false),
    pointsMinimumRange(0),
    pointsMaximumRange(0),
    pointsColorGradient(ColorGradientTemplate::defaultTemplateName),
    pointsInvertColorGradient(false),
    pointsOpacity(100),
    pointsSize(1),
    pointsLegend(false),
    vectorsMinimumRange(0),
    vectorsMaximumRange(0),
    useDefaultVectorsValues(true),
    vectorColorMode(VectorColorMode::MAGNITUDE),
    vectorsColorGradient(ColorGradientTemplate::defaultTemplateName),
    vectorsColor("#000000"),
    vectorsInvertColorGradient(true),
    vectorsOpacity(100),
    vectorsScale(0.5),
    vectorsWidth(1),
    vectorsLegend(true),
    meshLineColor("#000000"),
    meshLineStyle(0xFFFF),
    meshLineWidth(1),
    meshOpacity(100),
    useDefaultMapValues(true),
    useDefaultPointsValues(true)
{}

double LayerProperties::getMapMininumRange() const {
    return mapMinimumRange;
}

void LayerProperties::setMapMinimumRange(const double &mapMinimumRange) {
    this->mapMinimumRange = mapMinimumRange;
}

double LayerProperties::getMapMaximumRange() const {
    return mapMaximumRange;
}

void LayerProperties::setMapMaximumRange(const double &mapMaximumRange) {
    this->mapMaximumRange = mapMaximumRange;
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

double LayerProperties::getPointsMininumRange() const {
    return pointsMinimumRange;
}

void LayerProperties::setPointsMinimumRange(const double &pointsMinimumRange) {
    this->pointsMinimumRange = pointsMinimumRange;
}

double LayerProperties::getPointsMaximumRange() const {
    return pointsMaximumRange;
}

void LayerProperties::setPointsMaximumRange(const double &pointsMaximumRange) {
    this->pointsMaximumRange = pointsMaximumRange;
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

double LayerProperties::getVectorsMinimumRange() const {
    return vectorsMinimumRange;
}

void LayerProperties::setVectorsMinimumRange(const double &vectorsMinimumRange) {
    this->vectorsMinimumRange = vectorsMinimumRange;
}

double LayerProperties::getVectorsMaximumRange() const {
    return vectorsMaximumRange;
}

void LayerProperties::setVectorsMaximumRange(const double &vectorsMaximumRange) {
    this->vectorsMaximumRange = vectorsMaximumRange;
}

bool LayerProperties::getUseDefaultVectorsValues() const {
    return useDefaultVectorsValues;
}

void LayerProperties::setUseDefaultVectorsValues(const bool &useDefaultVectorsValues) {
    this->useDefaultVectorsValues = useDefaultVectorsValues;
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

bool LayerProperties::getUseDefaultMapValues() const {
    return useDefaultMapValues;
}

void LayerProperties::setUseDefaultMapValues(bool useDefaultMapValues) {
    this->useDefaultMapValues = useDefaultMapValues;
}

double LayerProperties::getDefaultMapMinimum() const {
    return defaultMapMinimum;
}

void LayerProperties::setDefaultMapMinimum(const double &defaultMapMinimum) {
    this->defaultMapMinimum = defaultMapMinimum;
}

double LayerProperties::getDefaultMapMaximum() const {
    return defaultMapMaximum;
}

void LayerProperties::setDefaultMapMaximum(const double &defaultMapMaximum) {
    this->defaultMapMaximum = defaultMapMaximum;
}

bool LayerProperties::getUseDefaultPointsValues() const {
    return useDefaultPointsValues;
}

void LayerProperties::setUseDefaultPointsValues(bool useDefaultPointsValues) {
    this->useDefaultPointsValues = useDefaultPointsValues;
}

double LayerProperties::getDefaultPointsMinimum() const {
    return defaultPointsMinimum;
}

void LayerProperties::setDefaultPointsMinimum(const double &defaultPointsMinimum) {
    this->defaultPointsMinimum = defaultPointsMinimum;
}

double LayerProperties::getDefaultPointsMaximum() const {
    return defaultPointsMaximum;
}

void LayerProperties::setDefaultPointsMaximum(const double &defaultPointsMaximum) {
    this->defaultPointsMaximum = defaultPointsMaximum;
}

double LayerProperties::getDefaultVectorsMinimum() const {
    return defaultVectorsMinimum;
}

void LayerProperties::setDefaultVectorsMinimum(const double &defaultVectorsMinimum) {
    this->defaultVectorsMinimum = defaultVectorsMinimum;
}

double LayerProperties::getDefaultVectorsMaximum() const {
    return defaultVectorsMaximum;
}

void LayerProperties::setDefaultVectorsMaximum(const double &defaultVectorsMaximum) {
    this->defaultVectorsMaximum = defaultVectorsMaximum;
}