#ifndef COLOR_GRADIENT_H
#define COLOR_GRADIENT_H

#include <QList>
#include <QColor>
#include <QString>
#include <QStringList>

class ColorGradient {
private:
    QString name;
    QString colors; // comma separated hex values
public:
    ColorGradient(const QString &name, const QString &colors) : name(name), colors(colors) {}

    QString getName() const {
        return name;
    }

    void setName(const QString &name) {
        this->name = name;
    }

    QString getColors() const {
        return colors;
    }

    void setColors(const QString &colors) {
        this->colors = colors;
    }
};

class ColorGrandientTemplate {
private:
    static QList<ColorGradient> colorGradients;
public:
    static QList<QColor> getColors(const QString &name) {
        QList<ColorGradient>::const_iterator selectedColorGradient = colorGradients.end();
        
        for (QList<ColorGradient>::const_iterator it = colorGradients.begin(); it != colorGradients.end(); it++) {
            if (it->getName() == name) {
                selectedColorGradient = it;
                break;
            }
        }

        QList<QColor> colors;

        if (selectedColorGradient != colorGradients.end()) {
            QStringList colorsStr = selectedColorGradient->getColors().split(",");

            for (int i = 0; i < colorsStr.size(); i++) {
                colors << QColor(colorsStr[i]);
            }
        }

        return colors;
    }
};

QList<ColorGradient> ColorGrandientTemplate::colorGradients = {
    ColorGradient("Blues", "#FFFFFF,#103B7B"),
    ColorGradient("BrBG", "#6B3F15,#FFFFFF,#16342C"),
    ColorGradient("Greens", "#FFFFFF,#6FAD60,#193918"),
    ColorGradient("Greys", "#FFFFFF,#7D7D7D,#0A0A0A"),
    ColorGradient("Oranges", "#FFFFFF,#E67A38,#6A2813"),
    ColorGradient("Paired", "#96C3DD,#3284BA,#8DC495,#80C566,#4D9F3C,#E19B8B,#F16869,#E53028,#F9AA66,#F8851D,#D0ACBB,#916DB3,#8F6F9A,#F7EE8E,#C17B41"),
    ColorGradient("PuOr", "#86410A,#F0F1F4,#370A59"),
    ColorGradient("RdYlBu", "#AB0929,#F8FCCA,#38459B"),
    ColorGradient("Reds", "#FFFFFF,#F86347,#750712"),
    ColorGradient("Bluehot", "#040409,#043BB8,#F1F7FF"),
    ColorGradient("Caleblack", "#040422,#04B3FF,#04FF1C,#FF4E04,#FF04CB"),
    ColorGradient("Contoured", "#0000FC,#02FD02,#FBFC02,#FE0303"),
    ColorGradient("Difference", "#0C0CFF,#FFFFFF,#FF0C0C"),
    ColorGradient("Gray", "#070707,#787878,#FFFFFF"),
    ColorGradient("Hot", "#0414FF,#04ECFF,#04FF15,#ECFF04,#FF1504"),
    ColorGradient("Rainbow", "#E200FB,#1900FB,#00FB3E,#FBFB00,#FB1500"),
    ColorGradient("Levels", "#FFFFFF,#767676,#040404")
};

#endif // COLOR_GRADIENT_H
