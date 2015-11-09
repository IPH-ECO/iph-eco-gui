#include "include/domain/layer_properties.h"

LayerProperties::LayerProperties() : 
    mapColorGradient(ColorGradientTemplate::defaultTemplateName), mapInvertColorGradient(false), mapOpacity(100), mapLegend(true), mapLighting(false),
    pointsColorGradient(ColorGradientTemplate::defaultTemplateName), pointsInvertColorGradient(false), pointsOpacity(100), pointsSize(1), pointsLegend(false),
    meshLineColor("#000000"), meshLineStyle(0xFFFF), meshLineWidth(1), meshOpacity(100)
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