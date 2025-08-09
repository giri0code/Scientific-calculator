#include <gtk/gtk.h>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream> // For debugging

class Calculator {
private:
    GtkWidget *window;
    GtkWidget *display;
    GtkWidget *history_display; // New: for showing full expression/previous result
    GtkWidget *grid;
    
    std::string current_input;
    std::string stored_value;
    std::string current_operation; // Renamed from 'operation'
    std::string full_expression;   // New: to build the expression string
    bool new_calculation;
    bool operator_pressed; // New: flag to manage input after an operator
    bool equals_pressed;   // New: flag to manage input after equals
    double memory_value;   // New: for memory functions
    
public:
    Calculator() : 
        current_input("0"), 
        stored_value(""), 
        current_operation(""), 
        full_expression(""),
        new_calculation(true),
        operator_pressed(false),
        equals_pressed(false),
        memory_value(0.0) {}
    
    void create_window() {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Scientific Calculator");
        gtk_window_set_default_size(GTK_WINDOW(window), 450, 600);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE); // Disable resizing
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); // Center on screen
        
        // Enable all window decorations (close, minimize, maximize buttons)
        gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
        gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_NORMAL);
        gtk_window_set_deletable(GTK_WINDOW(window), TRUE); // Enable close button
        
        // Set window icon (optional - will use default if no icon found)
        GError *error = NULL;
        GdkPixbuf *icon = gdk_pixbuf_new_from_file("/usr/share/icons/hicolor/48x48/apps/accessories-calculator.png", &error);
        if (icon) {
            gtk_window_set_icon(GTK_WINDOW(window), icon);
            g_object_unref(icon);
        }
        
        // Make window draggable by clicking anywhere in the title area
        // Note: gtk_window_set_has_resize_grip is deprecated in GTK3, resize grip is automatic
        
        // Connect window close event
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_signal_connect(window, "delete-event", G_CALLBACK(on_window_close), this);
        
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(window), vbox);
        gtk_container_set_border_width(GTK_CONTAINER(window), 10);
        
        // Create menu bar
        create_menu_bar(vbox);
        
        // Create history display (GtkLabel)
        history_display = gtk_label_new("");
        gtk_label_set_xalign(GTK_LABEL(history_display), 1.0); // Right align
        gtk_label_set_ellipsize(GTK_LABEL(history_display), PANGO_ELLIPSIZE_START); // Ellipsize long text
        gtk_widget_set_size_request(history_display, -1, 30);
        
        GtkCssProvider *history_css_provider = gtk_css_provider_new();
        const char *history_css_data = 
            "label { "
            "    font-size: 14px; "
            "    color: #888888; " // Lighter color for history
            "    padding-right: 5px; "
            "}";
        gtk_css_provider_load_from_data(history_css_provider, history_css_data, -1, NULL);
        GtkStyleContext *history_context = gtk_widget_get_style_context(history_display);
        gtk_style_context_add_provider(history_context, GTK_STYLE_PROVIDER(history_css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        gtk_box_pack_start(GTK_BOX(vbox), history_display, FALSE, FALSE, 0);

        // Create main display (GtkEntry)
        display = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(display), "0");
        gtk_entry_set_alignment(GTK_ENTRY(display), 1.0); // Right align
        gtk_editable_set_editable(GTK_EDITABLE(display), FALSE);
        gtk_widget_set_size_request(display, -1, 60); // Taller display
        
        GtkCssProvider *display_css_provider = gtk_css_provider_new();
        const char *display_css_data = 
            "entry { "
            "    font-size: 28px; " // Larger font
            "    font-weight: bold; "
            "    background-color: #f0f0f0; "
            "    border: 2px solid #ccc; "
            "    padding: 5px; "
            "}";
        gtk_css_provider_load_from_data(display_css_provider, display_css_data, -1, NULL);
        GtkStyleContext *display_context = gtk_widget_get_style_context(display);
        gtk_style_context_add_provider(display_context, GTK_STYLE_PROVIDER(display_css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        
        gtk_box_pack_start(GTK_BOX(vbox), display, FALSE, FALSE, 5); // Add some spacing below display
        
        // Create button grid
        grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 8); // Increased spacing
        gtk_grid_set_column_spacing(GTK_GRID(grid), 8); // Increased spacing
        gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
        
        create_buttons();
        
        // Add keyboard event handling
        g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), this);
        gtk_widget_set_can_focus(window, TRUE);
        
        gtk_widget_show_all(window);
        
        // Set minimum window size
        gtk_widget_set_size_request(window, 400, 500);
    }
    
    void create_menu_bar(GtkWidget *vbox) {
        GtkWidget *menubar = gtk_menu_bar_new();
        
        // File menu
        GtkWidget *file_menu = gtk_menu_new();
        GtkWidget *file_item = gtk_menu_item_new_with_label("File");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
        
        GtkWidget *exit_item = gtk_menu_item_new_with_label("Exit");
        g_signal_connect(exit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item);
        
        // View menu
        GtkWidget *view_menu = gtk_menu_new();
        GtkWidget *view_item = gtk_menu_item_new_with_label("View");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
        
        GtkWidget *always_on_top_item = gtk_check_menu_item_new_with_label("Always on Top");
        g_signal_connect(always_on_top_item, "toggled", G_CALLBACK(on_always_on_top_toggled), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), always_on_top_item);
        
        // Help menu
        GtkWidget *help_menu = gtk_menu_new();
        GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
        
        GtkWidget *about_item = gtk_menu_item_new_with_label("About");
        g_signal_connect(about_item, "activate", G_CALLBACK(on_about_clicked), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);
        
        // Add menus to menu bar
        gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help_item);
        
        gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    }
    
    void create_buttons() {
        // Button layout (8 rows, 5 columns for scientific calculator)
        const char* button_labels[8][5] = {
            {"AC", "CE", "sin", "cos", "tan"},
            {"log", "ln", "√", "x²", "xʸ"},
            {"π", "e", "!", "%", "÷"},
            {"7", "8", "9", "(", "×"},
            {"4", "5", "6", ")", "-"},
            {"1", "2", "3", "±", "+"},
            {"0", "00", ".", "⌫", "="},
            {"M+", "M-", "MR", "MC", ""}
        };
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 5; col++) {
                if (strlen(button_labels[row][col]) == 0) continue;
                
                GtkWidget *button = gtk_button_new_with_label(button_labels[row][col]);
                gtk_widget_set_size_request(button, 70, 60); // Standard size for all buttons
                gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
                
                style_button(button, button_labels[row][col]);
                g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), this);
            }
        }
    }
    
    void style_button(GtkWidget *button, const char *label) {
        GtkCssProvider *css_provider = gtk_css_provider_new();
        std::string css_data;
        
        // Improved color palette with better contrast
        if (strcmp(label, "AC") == 0 || strcmp(label, "CE") == 0) {
            // Clear buttons (bright red, white text)
            css_data = "button { "
                      "    font-size: 16px; "
                      "    font-weight: bold; "
                      "    background-color: #FF4444; " // Bright red
                      "    color: white; "
                      "    border-radius: 8px; "
                      "    border: 1px solid #CC0000; "
                      "}";
        } else if (strcmp(label, "sin") == 0 || strcmp(label, "cos") == 0 || strcmp(label, "tan") == 0 ||
                   strcmp(label, "log") == 0 || strcmp(label, "ln") == 0 || strcmp(label, "√") == 0 ||
                   strcmp(label, "x²") == 0 || strcmp(label, "xʸ") == 0 || strcmp(label, "π") == 0 ||
                   strcmp(label, "e") == 0 || strcmp(label, "!") == 0 || strcmp(label, "(") == 0 ||
                   strcmp(label, ")") == 0) {
            // Scientific function buttons (purple, white text)
            css_data = "button { "
                      "    font-size: 14px; "
                      "    font-weight: bold; "
                      "    background-color: #6A0DAD; " // Purple
                      "    color: white; "
                      "    border-radius: 8px; "
                      "    border: 1px solid #4B0082; "
                      "}";
        } else if (strcmp(label, "±") == 0 || strcmp(label, "%") == 0 || strcmp(label, "⌫") == 0 ||
                   strcmp(label, "M+") == 0 || strcmp(label, "M-") == 0 || 
                   strcmp(label, "MR") == 0 || strcmp(label, "MC") == 0) {
            // Utility buttons (light blue, dark text)
            css_data = "button { "
                      "    font-size: 16px; "
                      "    font-weight: bold; "
                      "    background-color: #87CEEB; " // Sky blue
                      "    color: #000080; " // Dark blue text
                      "    border-radius: 8px; "
                      "    border: 1px solid #4682B4; "
                      "}";
        } else if (strcmp(label, "÷") == 0 || strcmp(label, "×") == 0 || 
                   strcmp(label, "-") == 0 || strcmp(label, "+") == 0) {
            // Operation buttons (bright orange, white text)
            css_data = "button { "
                      "    font-size: 20px; "
                      "    font-weight: bold; "
                      "    background-color: #FF8C00; " // Dark orange
                      "    color: white; "
                      "    border-radius: 8px; "
                      "    border: 1px solid #FF6600; "
                      "}";
        } else if (strcmp(label, "=") == 0) {
            // Equals button (green, white text)
            css_data = "button { "
                      "    font-size: 22px; "
                      "    font-weight: bold; "
                      "    background-color: #32CD32; " // Lime green
                      "    color: white; "
                      "    border-radius: 8px; "
                      "    border: 1px solid #228B22; "
                      "}";
        } else {
            // Number and decimal buttons (dark blue, white text)
            css_data = "button { "
                      "    font-size: 18px; "
                      "    font-weight: bold; "
                      "    background-color: #2F4F4F; " // Dark slate gray
                      "    color: white; "
                      "    border-radius: 8px; "
                      "    border: 1px solid #1C1C1C; "
                      "}";
        }
        
        // Add hover effects
        css_data += "button:hover { opacity: 0.8; } ";
        css_data += "button:active { opacity: 0.6; }";
        
        gtk_css_provider_load_from_data(css_provider, css_data.c_str(), -1, NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(button);
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    
    static void on_button_clicked(GtkWidget *widget, gpointer data) {
        Calculator *calc = static_cast<Calculator*>(data);
        const char *label = gtk_button_get_label(GTK_BUTTON(widget));
        calc->handle_button_click(label);
    }
    
    // Window close callback
    static gboolean on_window_close(GtkWidget *widget, GdkEvent *event, gpointer data) {
        (void)widget;  // Suppress unused parameter warning
        (void)event;   // Suppress unused parameter warning
        (void)data;    // Suppress unused parameter warning
        // Perform cleanup if needed
        gtk_main_quit();
        return FALSE; // Allow the window to close
    }
    
    // Keyboard event handler
    static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
        (void)widget;  // Suppress unused parameter warning
        Calculator *calc = static_cast<Calculator*>(data);
        
        // Handle keyboard shortcuts
        switch (event->keyval) {
            case GDK_KEY_0: case GDK_KEY_KP_0: calc->handle_button_click("0"); break;
            case GDK_KEY_1: case GDK_KEY_KP_1: calc->handle_button_click("1"); break;
            case GDK_KEY_2: case GDK_KEY_KP_2: calc->handle_button_click("2"); break;
            case GDK_KEY_3: case GDK_KEY_KP_3: calc->handle_button_click("3"); break;
            case GDK_KEY_4: case GDK_KEY_KP_4: calc->handle_button_click("4"); break;
            case GDK_KEY_5: case GDK_KEY_KP_5: calc->handle_button_click("5"); break;
            case GDK_KEY_6: case GDK_KEY_KP_6: calc->handle_button_click("6"); break;
            case GDK_KEY_7: case GDK_KEY_KP_7: calc->handle_button_click("7"); break;
            case GDK_KEY_8: case GDK_KEY_KP_8: calc->handle_button_click("8"); break;
            case GDK_KEY_9: case GDK_KEY_KP_9: calc->handle_button_click("9"); break;
            case GDK_KEY_plus: case GDK_KEY_KP_Add: calc->handle_button_click("+"); break;
            case GDK_KEY_minus: case GDK_KEY_KP_Subtract: calc->handle_button_click("-"); break;
            case GDK_KEY_asterisk: case GDK_KEY_KP_Multiply: calc->handle_button_click("×"); break;
            case GDK_KEY_slash: case GDK_KEY_KP_Divide: calc->handle_button_click("÷"); break;
            case GDK_KEY_Return: case GDK_KEY_KP_Enter: case GDK_KEY_equal: 
                calc->handle_button_click("="); break;
            case GDK_KEY_period: case GDK_KEY_KP_Decimal: calc->handle_button_click("."); break;
            case GDK_KEY_BackSpace: calc->handle_button_click("⌫"); break;
            case GDK_KEY_Escape: calc->handle_button_click("AC"); break;
            case GDK_KEY_Delete: calc->handle_button_click("CE"); break;
            case GDK_KEY_percent: calc->handle_button_click("%"); break;
            case GDK_KEY_parenleft: calc->handle_button_click("("); break;
            case GDK_KEY_parenright: calc->handle_button_click(")"); break;
        }
        calc->update_display();
        return TRUE;
    }
    
    // Menu callbacks
    static void on_always_on_top_toggled(GtkCheckMenuItem *item, gpointer data) {
        Calculator *calc = static_cast<Calculator*>(data);
        gboolean active = gtk_check_menu_item_get_active(item);
        gtk_window_set_keep_above(GTK_WINDOW(calc->window), active);
    }
    
    static void on_about_clicked(GtkMenuItem *item, gpointer data) {
        (void)item;  // Suppress unused parameter warning
        Calculator *calc = static_cast<Calculator*>(data);
        
        GtkWidget *dialog = gtk_about_dialog_new();
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Scientific Calculator");
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
            "A feature-rich scientific calculator with keyboard support");
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "© 2025");
        
        const char* authors[] = {"Calculator Developer", NULL};
        gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
        
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(calc->window));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    
    void handle_button_click(const char *label) {
        std::string btn = label;
        
        if (btn >= "0" && btn <= "9") {
            handle_number(btn);
        } else if (btn == "00") {
            handle_number("00");
        } else if (btn == ".") {
            handle_decimal();
        } else if (btn == "AC") { // All Clear
            handle_all_clear();
        } else if (btn == "CE") { // Clear Entry
            handle_clear_entry();
        } else if (btn == "±") {
            handle_sign_change();
        } else if (btn == "%") {
            handle_percentage();
        } else if (btn == "⌫") { // Backspace
            handle_backspace();
        } else if (btn == "+" || btn == "-" || btn == "×" || btn == "÷" || btn == "^") {
            handle_operation(btn);
        } else if (btn == "=") {
            handle_equals();
        } else if (btn == "M+") {
            handle_memory_add();
        } else if (btn == "M-") {
            handle_memory_subtract();
        } else if (btn == "MR") {
            handle_memory_recall();
        } else if (btn == "MC") {
            handle_memory_clear();
        } else if (btn == "sin") {
            handle_scientific_function("sin");
        } else if (btn == "cos") {
            handle_scientific_function("cos");
        } else if (btn == "tan") {
            handle_scientific_function("tan");
        } else if (btn == "log") {
            handle_scientific_function("log");
        } else if (btn == "ln") {
            handle_scientific_function("ln");
        } else if (btn == "√") {
            handle_scientific_function("sqrt");
        } else if (btn == "x²") {
            handle_scientific_function("square");
        } else if (btn == "xʸ") {
            handle_operation("^");
        } else if (btn == "π") {
            handle_constant("pi");
        } else if (btn == "e") {
            handle_constant("e");
        } else if (btn == "!") {
            handle_scientific_function("factorial");
        } else if (btn == "(") {
            handle_parenthesis("(");
        } else if (btn == ")") {
            handle_parenthesis(")");
        }
        
        update_display();
    }
    
    void handle_number(const std::string &num) {
        if (new_calculation || operator_pressed || equals_pressed) {
            if (num == "00") {
                current_input = "0"; // Don't start with "00"
            } else {
                current_input = num;
            }
            if (equals_pressed) { // Clear full expression after equals
                full_expression = "";
            }
            new_calculation = false;
            operator_pressed = false;
            equals_pressed = false;
        } else {
            if (current_input == "0" && num != "00") {
                current_input = num; // Replace leading zero
            } else if (current_input == "0" && num == "00") {
                current_input = "0"; // Keep single zero
            } else {
                current_input += num;
            }
        }
        // Update full_expression for immediate display
        if (full_expression.empty() || (full_expression.back() >= '0' && full_expression.back() <= '9') || full_expression.back() == '.') {
            if (full_expression == "0" && num != ".") { // Replace initial 0 if not adding decimal
                full_expression = num;
            } else {
                full_expression += num;
            }
        } else {
            full_expression += num;
        }
    }
    
    void handle_decimal() {
        if (new_calculation || operator_pressed || equals_pressed) {
            current_input = "0.";
            if (equals_pressed) {
                full_expression = "";
            }
            new_calculation = false;
            operator_pressed = false;
            equals_pressed = false;
        } else if (current_input.find('.') == std::string::npos) {
            current_input += ".";
        }
        
        if (full_expression.find('.') == std::string::npos || (full_expression.back() != '.' && (full_expression.back() < '0' || full_expression.back() > '9'))) {
             if (full_expression.empty() || (full_expression.back() < '0' || full_expression.back() > '9')) {
                full_expression += "0.";
            } else {
                full_expression += ".";
            }
        }
    }
    
    void handle_all_clear() { // Renamed from handle_clear
        current_input = "0";
        stored_value = "";
        current_operation = "";
        full_expression = "";
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }

    void handle_clear_entry() { // New: Clear current input only
        current_input = "0";
        // If an operation is pending, clear the last number from full_expression
        if (!current_operation.empty() && !full_expression.empty()) {
            size_t last_num_start = full_expression.find_last_not_of("+-×÷%±⌫.0123456789");
            if (last_num_start != std::string::npos) {
                full_expression.erase(last_num_start + 1);
            } else {
                full_expression = ""; // If only a number, clear it
            }
        } else {
            full_expression = "";
        }
        new_calculation = true; // Ready for new input
    }

    void handle_backspace() { // New: Delete last character
        if (current_input.length() > 1 && current_input != "Error") {
            current_input.pop_back();
        } else {
            current_input = "0";
        }
        if (full_expression.length() > 0) {
            full_expression.pop_back();
        }
        if (full_expression.empty()) {
            full_expression = ""; // Ensure it's truly empty for history display
        }
        new_calculation = false; // Allow continued input
    }
    
    void handle_sign_change() {
        if (current_input != "0" && current_input != "Error") {
            if (current_input[0] == '-') {
                current_input = current_input.substr(1);
            } else {
                current_input = "-" + current_input;
            }
            // Update full_expression to reflect sign change
            if (!full_expression.empty()) {
                size_t last_num_start = full_expression.find_last_not_of("+-×÷%±⌫");
                if (last_num_start == std::string::npos) last_num_start = 0; else last_num_start++;
                
                std::string last_num_str = full_expression.substr(last_num_start);
                if (last_num_str[0] == '-') {
                    full_expression.replace(last_num_start, last_num_str.length(), last_num_str.substr(1));
                } else {
                    full_expression.replace(last_num_start, last_num_str.length(), "-" + last_num_str);
                }
            }
        }
    }
    
    void handle_percentage() {
        if (current_input == "Error") return;
        double value = std::stod(current_input);
        value /= 100.0;
        current_input = format_number(value);
        // Update full_expression
        if (!full_expression.empty()) {
            size_t last_num_start = full_expression.find_last_not_of("+-×÷%±⌫");
            if (last_num_start == std::string::npos) last_num_start = 0; else last_num_start++;
            full_expression.replace(last_num_start, full_expression.length() - last_num_start, current_input);
            full_expression += "%"; // Add percentage symbol to history
        }
    }
    
    void handle_operation(const std::string &op) {
        if (current_input == "Error") return;

        if (!current_operation.empty() && !operator_pressed) {
            // If there's a pending operation and a new number was entered, calculate first
            handle_equals();
            stored_value = current_input; // Result becomes the new stored value
        } else if (equals_pressed) {
            // If equals was just pressed, the result is already in current_input
            stored_value = current_input;
            full_expression = current_input; // Start new expression with the result
        } else {
            stored_value = current_input;
        }
        
        current_operation = op;
        operator_pressed = true;
        new_calculation = true; // Next number will clear current_input
        equals_pressed = false;

        // Append operation to full_expression, avoid double operators
        if (!full_expression.empty() && (full_expression.back() == '+' || full_expression.back() == '-' ||
                                         full_expression.find("×") == full_expression.length() - 1 || 
                                         full_expression.find("÷") == full_expression.length() - 1)) {
            // Remove the last operator and add the new one
            if (full_expression.find("×") == full_expression.length() - 1 || 
                full_expression.find("÷") == full_expression.length() - 1) {
                full_expression.pop_back(); // Remove the Unicode operator
            } else {
                full_expression.back() = op[0]; // Replace ASCII operator
            }
            full_expression += op;
        } else {
            full_expression += op;
        }
    }
    
    void handle_equals() {
        if (current_operation.empty() || stored_value.empty() || current_input == "Error") {
            return;
        }
        
        double val1 = std::stod(stored_value);
        double val2 = std::stod(current_input);
        double result = 0;
        
        if (current_operation == "+") {
            result = val1 + val2;
        } else if (current_operation == "-") {
            result = val1 - val2;
        } else if (current_operation == "×") {
            result = val1 * val2;
        } else if (current_operation == "÷") {
            if (val2 != 0) {
                result = val1 / val2;
            } else {
                current_input = "Error";
                full_expression = "Error: Division by zero";
                current_operation = "";
                stored_value = "";
                new_calculation = true;
                operator_pressed = false;
                equals_pressed = true; // Set equals_pressed to true for error state
                return;
            }
        } else if (current_operation == "^") {
            result = pow(val1, val2);
        }
        
        current_input = format_number(result);
        full_expression += " = " + current_input; // Complete the expression
        current_operation = "";
        stored_value = "";
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = true; // Indicate that equals was pressed
    }

    // New: Scientific function handlers
    void handle_scientific_function(const std::string &func) {
        if (current_input == "Error") return;
        
        double value = std::stod(current_input);
        double result = 0;
        
        if (func == "sin") {
            result = sin(value * M_PI / 180.0); // Convert to radians
            full_expression = "sin(" + current_input + ")";
        } else if (func == "cos") {
            result = cos(value * M_PI / 180.0);
            full_expression = "cos(" + current_input + ")";
        } else if (func == "tan") {
            result = tan(value * M_PI / 180.0);
            full_expression = "tan(" + current_input + ")";
        } else if (func == "log") {
            if (value > 0) {
                result = log10(value);
                full_expression = "log(" + current_input + ")";
            } else {
                current_input = "Error";
                full_expression = "Error: Invalid input for log";
                return;
            }
        } else if (func == "ln") {
            if (value > 0) {
                result = log(value);
                full_expression = "ln(" + current_input + ")";
            } else {
                current_input = "Error";
                full_expression = "Error: Invalid input for ln";
                return;
            }
        } else if (func == "sqrt") {
            if (value >= 0) {
                result = sqrt(value);
                full_expression = "√(" + current_input + ")";
            } else {
                current_input = "Error";
                full_expression = "Error: Invalid input for √";
                return;
            }
        } else if (func == "square") {
            result = value * value;
            full_expression = "(" + current_input + ")²";
        } else if (func == "factorial") {
            if (value >= 0 && value == floor(value) && value <= 20) { // Limit to avoid overflow
                result = 1;
                for (int i = 1; i <= (int)value; i++) {
                    result *= i;
                }
                full_expression = current_input + "!";
            } else {
                current_input = "Error";
                full_expression = "Error: Invalid input for factorial";
                return;
            }
        }
        
        current_input = format_number(result);
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }
    
    void handle_constant(const std::string &constant) {
        if (constant == "pi") {
            current_input = format_number(M_PI);
            full_expression = "π";
        } else if (constant == "e") {
            current_input = format_number(M_E);
            full_expression = "e";
        }
        
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }
    
    void handle_parenthesis(const std::string &paren) {
        // Simple parenthesis handling for display
        if (paren == "(") {
            if (new_calculation || operator_pressed || equals_pressed) {
                full_expression = "(";
                current_input = "0";
            } else {
                full_expression += "(";
            }
        } else if (paren == ")") {
            full_expression += ")";
        }
        
        new_calculation = false;
        operator_pressed = false;
        equals_pressed = false;
    }

    // New: Memory functions
    void handle_memory_add() {
        if (current_input == "Error") return;
        memory_value += std::stod(current_input);
        full_expression = "M+ " + current_input;
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }

    void handle_memory_subtract() {
        if (current_input == "Error") return;
        memory_value -= std::stod(current_input);
        full_expression = "M- " + current_input;
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }

    void handle_memory_recall() {
        current_input = format_number(memory_value);
        full_expression = "MR";
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }

    void handle_memory_clear() {
        memory_value = 0.0;
        full_expression = "MC";
        new_calculation = true;
        operator_pressed = false;
        equals_pressed = false;
    }
    
    std::string format_number(double num) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(10) << num;
        std::string result = oss.str();
        
        // Remove trailing zeros
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        // Remove trailing decimal point if it's the last character
        if (result.back() == '.') {
            result.pop_back();
        }
        
        return result;
    }
    
    void update_display() {
        gtk_entry_set_text(GTK_ENTRY(display), current_input.c_str());
        gtk_label_set_text(GTK_LABEL(history_display), full_expression.c_str());
    }
    
    void run() {
        gtk_main();
    }
};

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    Calculator calc;
    calc.create_window();
    calc.run();
    
    return 0;
}
