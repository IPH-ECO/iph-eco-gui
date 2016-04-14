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
    ColorGradient(const QString &name, const QString &colors);

    QString getName() const;
    void setName(const QString &name);
    QString getColors() const;
    void setColors(const QString &colors);
};

class ColorGradientTemplate {
public:
    static const QString defaultTemplateName;
    static const QList<ColorGradient> colorGradients;
    static QString getTemplateName(int index);
    static QList<QColor> getColors(int index);
    static QList<QColor> getColors(const QString &colorTemplateName);
};

#endif // COLOR_GRADIENT_H
