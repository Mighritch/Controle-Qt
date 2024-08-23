#include "DateDelegate.h"
#include <QDate>

DateDelegate::DateDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QString DateDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    if (value.type() == QVariant::Date) {
        return value.toDate().toString("dd/MM/yyyy");
    }
    return QStyledItemDelegate::displayText(value, locale);
}



