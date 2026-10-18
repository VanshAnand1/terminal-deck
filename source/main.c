#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <3ds.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

int main(int argc, char **argv) {
    gfxInitDefault();
    
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    u32 *SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    socInit(SOC_buffer, SOC_BUFFERSIZE);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5656);

    inet_pton(AF_INET, "172.20.10.10", &serv_addr.sin_addr); 

    // Draw initial blank UI
    consoleSelect(&bottomScreen);
    printf("\x1b[2;7H\x1b[42;37m                            \x1b[0m");
    printf("\x1b[3;7H\x1b[42;37m        CONNECTING...       \x1b[0m");
    printf("\x1b[4;7H\x1b[42;37m                            \x1b[0m");

    printf("\x1b[7;7H\x1b[44;37m                            \x1b[0m");
    printf("\x1b[8;7H\x1b[44;37m        CONNECTING...       \x1b[0m");
    printf("\x1b[9;7H\x1b[44;37m                            \x1b[0m");

    printf("\x1b[12;7H\x1b[45;37m                            \x1b[0m");
    printf("\x1b[13;7H\x1b[45;37m        CONNECTING...       \x1b[0m");
    printf("\x1b[14;7H\x1b[45;37m                            \x1b[0m");

    printf("\x1b[17;7H\x1b[46;37m                            \x1b[0m");
    printf("\x1b[18;7H\x1b[46;37m        CONNECTING...       \x1b[0m");
    printf("\x1b[19;7H\x1b[46;37m                            \x1b[0m");

    printf("\x1b[22;7H\x1b[41;37m                            \x1b[0m");
    printf("\x1b[23;7H\x1b[41;37m         CLEAR LOGS         \x1b[0m");
    printf("\x1b[24;7H\x1b[41;37m                            \x1b[0m");

    consoleSelect(&topScreen);
    printf("Connecting to Mac...\n");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed.\n");
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);
    touchPosition touch;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break; 
        
        if (kDown & KEY_TOUCH) {
            hidTouchRead(&touch);
            
            // 5 Tightly Calibrated Hitboxes
            if (touch.py >= 10 && touch.py <= 40) {
                send(sock, "CMD:0\n", 6, 0);
            } 
            else if (touch.py >= 50 && touch.py <= 80) {
                send(sock, "CMD:1\n", 6, 0);
            } 
            else if (touch.py >= 90 && touch.py <= 120) {
                send(sock, "CMD:2\n", 6, 0);
            } 
            else if (touch.py >= 130 && touch.py <= 160) {
                send(sock, "CMD:3\n", 6, 0);
            } 
            else if (touch.py >= 170 && touch.py <= 210) {
                consoleSelect(&topScreen);
                consoleClear();
                printf("Logs cleared.\n");
            }
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            // Intercept the dynamic UI payload
            char *init_ptr = strstr(buffer, "INIT:");
            if (init_ptr != NULL) {
                char b1[30], b2[30], b3[30], b4[30];
                sscanf(init_ptr, "INIT:%26[^|]|%26[^|]|%26[^|]|%26[^\n]", b1, b2, b3, b4);
                
                consoleSelect(&bottomScreen);
                printf("\x1b[3;7H\x1b[42;37m %s \x1b[0m", b1);
                printf("\x1b[8;7H\x1b[44;37m %s \x1b[0m", b2);
                printf("\x1b[13;7H\x1b[45;37m %s \x1b[0m", b3);
                printf("\x1b[18;7H\x1b[46;37m %s \x1b[0m", b4);
                
                consoleSelect(&topScreen);
                printf("UI Config loaded from Mac.\n");
            } else {
                // Normal terminal logs
                consoleSelect(&topScreen);
                printf("%s", buffer);
            }
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    close(sock);
    socExit();
    gfxExit();
    return 0;
}