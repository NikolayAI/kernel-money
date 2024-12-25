#define LINE_SIZE 160
#define LINE_COUNT 25
#define WHITE_BLUE 0x1F
#define WHITE_GREEN 0x2F
#define PASSWORD_LENGTH 4

unsigned int k_print_text(char *message, unsigned int line, int text_color);
void k_change_cursor_position(int row, int col);
void k_handle_text(char *buffer, int pos, unsigned int max_length);
void k_paint_display(int color);
static inline void k_asm_v_outb(unsigned short port, unsigned char value);
static inline unsigned char k_asm_v_inb(unsigned short port);

static inline unsigned char k_asm_v_inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
};

static inline void k_asm_v_outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "d"(port));
};

void k_change_cursor_position(int row, int col) {
    unsigned short position=(row * 80) + col;

    k_asm_v_outb(0x3D4, 14);
    k_asm_v_outb(0x3D5, (unsigned char)(position>>8));
    k_asm_v_outb(0x3D4, 15);
    k_asm_v_outb(0x3D5, (unsigned char)(position));
};

void k_paint_display(int color) {
    char *vidptr = (char *) 0xb8000;
    unsigned int i = 0;
    while (i < (LINE_SIZE * LINE_COUNT))
    {
        vidptr[i] = ' ';
        i++;
        vidptr[i] = color;
        i++;
    };
};

unsigned int k_print_text(char *message, unsigned int line, int text_color) {
    char *vidptr = (char *) 0xb8000;
    unsigned int i = 0;

    i = (line * LINE_SIZE);

    while (*message != 0)
    {
        if (*message == '\n')
        {
            line++;
            i = (line * LINE_SIZE);
            message++;
        }
        else
        {
            vidptr[i] = *message;
            message++;
            i++;
            vidptr[i] = text_color;
            i++;
        };
    };

    return 1;
};

void k_handle_text(char *buffer, int pos, unsigned int max_length) {
    unsigned int count = 0;
    char *vidptr = (char *) 0xb8000;

    while (1) {
        unsigned char scancode;

        do {
            scancode = k_asm_v_inb(0x60);
        } while (scancode & 0x80);

        char key = 0;
        switch (scancode) {
            case 0x02: key = '1'; break;
            case 0x03: key = '2'; break;
            case 0x04: key = '3'; break;
            case 0x05: key = '4'; break;
            case 0x06: key = '5'; break;
            case 0x07: key = '6'; break;
            case 0x08: key = '7'; break;
            case 0x09: key = '8'; break;
            case 0x0A: key = '9'; break;
            case 0x0B: key = '0'; break;
            case 0x1C:
                buffer[count] = '\0';
                return;
            case 0x0E:
                if (count > 0) {
                    count--;
                    pos -= 2;
                    vidptr[pos] = ' ';
                }
                continue;
            default:
                continue;
        }

        if (count < max_length) {
            buffer[count] = key;
            vidptr[pos] = key;
            vidptr[pos + 1] = WHITE_BLUE;
            count++;
            pos += 2;
			k_change_cursor_position(0, pos/2);
        }

        do {
            scancode = k_asm_v_inb(0x60);
        } while (!(scancode & 0x80));
    }
};

void k_main() {
    char password[PASSWORD_LENGTH];

    k_paint_display(WHITE_BLUE);
    k_print_text("Privet, eto nadeznyi service!", 0, WHITE_BLUE);
    k_print_text("Perevedi dengi nam, mi samie luchshie.", 1, WHITE_BLUE);
    k_print_text("Vvedi parol:", 3, WHITE_BLUE);
	k_change_cursor_position(3, 15);

    k_asm_v_outb(0x61, k_asm_v_inb(0x61) | 0x03);

	int pos = 3 * LINE_SIZE + 15 * 2;

    k_handle_text(password, pos, PASSWORD_LENGTH);

 	k_asm_v_outb(0x61, k_asm_v_inb(0x61) & 0xFC);

	k_paint_display(WHITE_GREEN);
    k_print_text("Krasava!", 0, WHITE_GREEN);
	k_print_text("Ti mozhesh ocenit nashe obsluzhivanie", 1, WHITE_GREEN);
	k_print_text("po pjatibalnoy shkale", 2, WHITE_GREEN);
	k_change_cursor_position(3, 0);

    while (1) {}
};
