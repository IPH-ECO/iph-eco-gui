#ifndef LAYER_PROPERTIES_H
#define LAYER_PROPERTIES_H

#include "color_gradient.h"
#include <QString>

enum class VectorColorMode { MAGNITUDE = 1, CONSTANT };

class LayerProperties {
private:
    double mapMinimumRange;
    double mapMaximumRange;
    QString mapColorGradient;
    bool mapInvertColorGradient;
    int mapOpacity;
    bool mapLegend;
    bool mapLighting;
    
    double pointsMinimumRange;
    double pointsMaximumRange;
    QString pointsColorGradient;
    bool pointsInvertColorGradient;
    int pointsOpacity;
    int pointsSize;
    bool pointsLegend;
    
    double vectorsMinimumRange;
    double vectorsMaximumRange;
    bool useDefaultVectorsValues;
    VectorColorMode vectorColorMode;
    QString vectorsColorGradient;
    QString vectorsColor;
    bool vectorsInvertColorGradient;
    int vectorsOpacity;
    double vectorsScale;
    int vectorsWidth;
    bool vectorsLegend;
    
    QString meshLineColor;
    int meshLineStyle;
    int meshLineWidth;
    int meshOpacity;

    // Transient attributes
    bool useDefaultMapValues;
    double defaultMapMinimum;
    double defaultMapMaximum;
    bool useDefaultPointsValues;
    double defaultPointsMinimum;
    double defaultPointsMaximum;
public:
	LayerProperties();

	double getMapMininumRange() const;
    void setMapMinimumRange(const double &mapMinimumRange);
    double getMapMaximumRange() const;
    void setMapMaximumRange(const double &mapMaximumRange);
    QString getMapColorGradient() const;
    void setMapColorGradient(const QString &mapColorGradient);
    bool getMapInvertColorGradient() const;
    void setMapInvertColorGradient(const bool &mapInvertColorGradient);
    int getMapOpacity() const;
    void setMapOpacity(const int &mapOpacity);
    bool getMapLegend() const;
    void setMapLegend(const bool &mapLegend);
    bool getMapLighting() const;
    void setMapLighting(const bool &mapLighting);
    
    double getPointsMininumRange() const;
    void setPointsMinimumRange(const double &pointsMinimumRange);
    double getPointsMaximumRange() const;
    void setPointsMaximumRange(const double &pointsMaximumRange);
    QString getPointsColorGradient() const;
    void setPointsColorGradient(const QString &pointsColorGradient);
    bool getPointsInvertColorGradient() const;
    void setPointsInvertColorGradient(const bool &pointsInvertColorGradient);
    int getPointsOpacity() const;
    void setPointsOpacity(const int &pointsOpacity);
    int getPointsSize() const;
    void setPointsSize(const int &pointsSize);
    bool getPointsLegend() const;
    void setPointsLegend(const bool &pointsLegend);
    
    double getVectorsMinimumRange() const;
    void setVectorsMinimumRange(const double &vectorsMinimumRange);
    double getVectorsMaximumRange() const;
    void setVectorsMaximumRange(const double &vectorsMaximumRange);
    bool getUseDefaultVectorsValues() const;
    void setUseDefaultVectorsValues(const bool &useDefaultVectorsValues);
    VectorColorMode getVectorColorMode() const;
    void setVectorColorMode(const VectorColorMode &vectorColorMode);
    QString getVectorsColorGradient() const;
    void setVectorsColorGradient(const QString &vectorsColorGradient);
    QString getVectorsColor() const;
    void setVectorsColor(const QString &vectorsColor);
    bool getVectorsInvertColorGradient() const;
    void setVectorsInvertColorGradient(const bool &vectorsInvertColorGradient);
    int getVectorsOpacity() const;
    void setVectorsOpacity(const int &vectorsOpacity);
    double getVectorsScale() const;
    void setVectorsScale(const double &vectorsScale);
    int getVectorsWidth() const;
    void setVectorsWidth(const int &vectorsWidth);
    bool getVectorsLegend() const;
    void setVectorsLegend(const bool &vectorsLegend);
    
    QString getMeshLineColor() const;
    void setMeshLineColor(const QString &meshLineColor);
    int getMeshLineStyle() const;
    void setMeshLineStyle(const int &meshLineStyle);
    int getMeshLineWidth() const;
    void setMeshLineWidth(const int &meshLineWidth);
    int getMeshOpacity() const;
    void setMeshOpacity(const int &meshOpacity);

    bool getUseDefaultMapValues() const;
    void setUseDefaultMapValues(bool useDefaultMapValues);
    double getDefaultMapMinimum() const;
    void setDefaultMapMinimum(const double &defaultMapMinimum);
    double getDefaultMapMaximum() const;
    void setDefaultMapMaximum(const double &defaultMapMaximum);
    bool getUseDefaultPointsValues() const;
    void setUseDefaultPointsValues(bool useDefaultPointsValues);
    double getDefaultPointsMinimum() const;
    void setDefaultPointsMinimum(const double &defaultPointsMinimum);
    double getDefaultPointsMaximum() const;
    void setDefaultPointsMaximum(const double &defaultPointsMaximum);
};

#endif // LAYER_PROPERTIES_H