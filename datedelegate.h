#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QStyledItemDelegate>

class DateDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit DateDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // DATEDELEGATE_H


