#ifndef LAYER_PROPERTIES_H
#define LAYER_PROPERTIES_H

#include "color_gradient.h"
#include <QString>

enum class VectorColorMode { MAGNITUDE, CONSTANT };

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
    
    QString meshLineColor;
    int meshLineStyle;
    int meshLineWidth;
    int meshOpacity;
    
    VectorColorMode vectorColorMode;
    QString vectorColor;
    int vectorWidth;
    double vectorScale;

    // Transient attributes
    double defaultMapMinimum;
    double defaultMapMaximum;
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
    
    QString getMeshLineColor() const;
    void setMeshLineColor(const QString &meshLineColor);
    int getMeshLineStyle() const;
    void setMeshLineStyle(const int &meshLineStyle);
    int getMeshLineWidth() const;
    void setMeshLineWidth(const int &meshLineWidth);
    int getMeshOpacity() const;
    void setMeshOpacity(const int &meshOpacity);

    double getDefaultMapMinimum() const;
    void setDefaultMapMinimum(const double &defaultMapMinimum);
    double getDefaultMapMaximum() const;
    void setDefaultMapMaximum(const double &defaultMapMaximum);
    double getDefaultPointsMinimum() const;
    void setDefaultPointsMinimum(const double &defaultPointsMinimum);
    double getDefaultPointsMaximum() const;
    void setDefaultPointsMaximum(const double &defaultPointsMaximum);
    
    VectorColorMode getVectorColorMode() const;
    void setVectorColorMode(const VectorColorMode &vectorColorMode);
    QString getVectorColor() const;
    void setVectorColor(const QString &vectorColor);
    int getVectorWidth() const;
    void setVectorWidth(int vectorWidth);
    double getVectorScale() const;
    void setVectorScale(double vectorScale);
};

#endif // LAYER_PROPERTIES_H