#ifndef QT_VIEW_MKO_H__

#define QT_VIEW_MKO_H__

#include <cstdint>
#include <memory>
#include <vector>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include "view.h"

class MkoClient;
struct OuSubaddressData;

class QtViewMKO : public View
{
public:
    explicit QtViewMKO(QStackedWidget *const stacked_widget) noexcept;
    QtViewMKO(const QtViewMKO &other) = delete;
    QtViewMKO(QtViewMKO &&other) = delete;

    ~QtViewMKO() = default;

    QtViewMKO &operator=(const QtViewMKO &other) = delete;
    QtViewMKO &operator=(QtViewMKO &&other) = delete;

    void show() override;
private:
    static constexpr int NOT_FOUND{-1};
    static constexpr int WORD_COUNT{32};
    static constexpr const char *DEFAULT_MKO_SERVICE_ADDRESS{"127.0.0.1:50051"};

    QWidget *central_widget;
    QStackedWidget *stacked_widget;
    QTextEdit *operation_log;
    QSpinBox *kk_index_spin_box;
    QSpinBox *ou_index_spin_box;
    std::shared_ptr<MkoClient> mko_client;
    bool is_updating_mko_indices;

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;

    QLabel *create_label(const QString &text) const;

    QWidget *create_kk_panel();

    QWidget *create_ou_panel();

    QWidget *create_menu_panel();

    QWidget *create_log_panel();

    QGroupBox *create_group_box(const QString &title) const;

    QLineEdit *create_line_edit(const QString &placeholder = QString{}) const;

    QSpinBox *create_spin_box(int minimum, int maximum, int value = 0) const;

    QComboBox *create_channel_combo_box() const;

    QTableWidget *create_words_table() const;

    void add_labeled_widget(QGridLayout *const layout, int row, const QString &label_text, QWidget *const widget) const;

    std::vector<uint32_t> collect_words(const QTableWidget *const table) const;

    QString format_word(uint32_t word) const;

    QString format_words(const std::vector<uint32_t> &words) const;

    void append_ou_subaddress_data(const OuSubaddressData &data);

    void sync_mko_indices(QSpinBox *const changed_spin_box);

    static int alternate_mko_index(int index);

    void reset_parameters_to_defaults();

    void append_log(const QString &message);
};

#endif
