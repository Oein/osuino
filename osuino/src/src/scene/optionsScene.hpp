enum OptionsSceneCursor
{
    Options_WiFi,
    Options_Bluetooth,
    Options_Count
};

OptionsSceneCursor viewingOptPage = Options_Count;

class WiFiScene
{
public:
    CnavasAPI *api;
    KeyHelper *keyHelper;
    
    AnimatedData yOffset = AnimatedData(0, 50);
    AnimatedData opacity = AnimatedData(100, 50);

    RepeatedButton button0;
    RepeatedButton button1;
    bool last2Button = false;
    bool last3Button = false;

    bool scaned = false;
    int scanCount = 0;
    int selectedNetwork = 0; // Cursor position for network selection
    int scrollOffset = 0;    // For scrolling when there are many networks
    
    WiFiScene(CnavasAPI *api)
    {
        this->api = api;
        this->keyHelper = new KeyHelper(api);
    }
    
    void renderBase()
    {
        api->clear();
        api->drawRect(0, 0, CANVAS_WIDTH, 40, rgb(10, 50, 50));
        api->drawTextTopLeft(10, 20, "WiFi Settings", COLOR_WHITE);
        
        keyHelper->setBtn(true, true, true, true);
        keyHelper->setBtnText("Up", "Down", "Connect", "Back");
        keyHelper->render();
    }

    int constrain(int value, int min, int max)
    {
        if(value < min) return min;
        if(value > max) return max;
        return value;
    }

    int mapper(int value, int fromLow, int fromHigh, int toLow, int toHigh)
    {
        return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
    }

    void render() {
        renderBase();
        
        if(scanCount == 0) {
            api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "No networks found", COLOR_WHITE);
            return;
        }
        
        // Display the networks
        int displayCount = scanCount; // Show max 5 networks at once
        if(displayCount > 5) displayCount = 5;
        int startIdx = selectedNetwork - 2;
        if(startIdx > scanCount - displayCount) startIdx = scanCount - displayCount;
        if(startIdx < 0) startIdx = 0;
        
        for(int i = 0; i < displayCount; i++) {
            int networkIdx = i + startIdx;
            int yPos = 50 + i * 45;
            
            // Draw selection background
            if(networkIdx == selectedNetwork) {
                api->drawRect(10 - 3, yPos - 3, CANVAS_WIDTH - 20 + 6, 40 + 6, COLOR_OSU);
                api->drawRect(10, yPos, CANVAS_WIDTH - 20, 40, rgb_darken(255, 255, 255, opacity.current() / 100));
            } else {
                api->drawRect(10, yPos, CANVAS_WIDTH - 20, 40, rgb_darken(255, 255, 255, 0.3));
            }
            
            // Draw network name
            IStringType ssid = WiFi.SSID(networkIdx);
            if(ssid.length() > 18) ssid = subString(ssid, 0, 15) + "...";
            
            api->drawTextTopLeft(15, yPos + 15, ssid.c_str(), COLOR_BLACK);
            
            // Draw signal strength indicator (simple bars)
            int rssi = WiFi.RSSI(networkIdx);
            int strength = mapper(constrain(rssi, -100, -40), -100, -40, 0, 4);
            
            for(int j = 0; j < 4; j++) {
                uint16_t barColor = j < strength ? rgb(0, 150, 0) : rgb(100, 100, 100);
                api->drawRect(CANVAS_WIDTH - 35 + (j * 6), yPos + 20 - (j * 3), 4, 5 + (j * 3), barColor);
            }
            
            // Draw lock icon for secured networks
            if(WiFi.encryptionType(networkIdx) != WIFI_AUTH_OPEN) {
                api->drawRect(CANVAS_WIDTH - 55, yPos + 15, 8, 8, COLOR_BLACK);
                api->drawRect(CANVAS_WIDTH - 60, yPos + 23, 18, 10, COLOR_BLACK);
            }
        }
        
        // Show scroll indicators if needed
        if(startIdx > 0) {
            api->drawTriangle(CANVAS_WIDTH/2 - 5, 45, CANVAS_WIDTH/2 + 5, 45, CANVAS_WIDTH/2, 40, rgb(150, 150, 150));
        }
        if(startIdx + displayCount < scanCount) {
            api->drawTriangle(CANVAS_WIDTH/2 - 5, CANVAS_HEIGHT - 45, CANVAS_WIDTH/2 + 5, CANVAS_HEIGHT - 45, CANVAS_WIDTH/2, CANVAS_HEIGHT - 40, rgb(150, 150, 150));
        }
        
        // Show network count at bottom
        char countText[20];
        sprintf(countText, "%d networks found", scanCount);
        api->drawTextTopLeft(CANVAS_WIDTH/2, CANVAS_HEIGHT - 50, countText, rgb(150, 150, 150));
    }

    bool showingKeyboard = false;
    IStringType password = "";

    void connect() {
        renderBase();
        api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Connecting...", COLOR_WHITE);

        // WL_NO_SSID_AVAIL
        // WL_CONNECT_FAILED
        // WL_CONNECTION_LOST
        // WL_DISCONNECTED
        WiFi.begin(WiFi.SSID(selectedNetwork).c_str(), password.c_str());
        while(WiFi.status() != WL_CONNECTED) {
            if(WiFi.status() == WL_NO_SSID_AVAIL) {
                renderBase();
                api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "No network found", COLOR_WHITE);
                return;
            }
            if(WiFi.status() == WL_CONNECT_FAILED) {
                renderBase();
                api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Connection failed", COLOR_WHITE);
                return;
            }
            if(WiFi.status() == WL_CONNECTION_LOST) {
                renderBase();
                api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Connection lost", COLOR_WHITE);
                return;
            }
            if(WiFi.status() == WL_DISCONNECTED) {
                renderBase();
                api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Disconnected", COLOR_WHITE);
                return;
            }
        }

        renderBase();
        api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Connected!", COLOR_WHITE);
    }
    
    void update(bool forceRender)
    {
        if(showingKeyboard) {
            input.update();
            if(input.result.handled == HnadledType::CANCELED) {
                showingKeyboard = false;
                input.result.handled = HnadledType::NONE;
                return;
            }
            if(input.result.handled == HnadledType::HANDLED) {
                password = input.result.result;
                showingKeyboard = false;
                connect();
                return;
            }
            return;
        }

        bool updated = forceRender;
        updated |= yOffset.update();
        updated |= opacity.update();

        if(forceRender || !scaned) {
            renderBase();
            // print scanning
            scaned = true;
            api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Scanning...", COLOR_WHITE);
            scanCount = WiFi.scanNetworks();
            selectedNetwork = 0;
            updated = true;
        }
        
        // Handle up/down navigation
        if(button0.get(buttonPressed(0)) && selectedNetwork > 0) {
            selectedNetwork--;
            opacity.set(60);
            opacity.setTarget(100);
            updated = true;
        }
        
        if(button1.get(buttonPressed(1)) && selectedNetwork < scanCount - 1) {
            selectedNetwork++;
            opacity.set(60);
            opacity.setTarget(100);
            updated = true;
        }
        
        // Handle selection button (connect to network)
        if(last2Button != buttonPressed(2)) {
            last2Button = buttonPressed(2);
            if(!last2Button && scanCount > 0) {
                // Show connecting message
                renderBase();

                // has password?
                bool hasPassword = WiFi.encryptionType(selectedNetwork) != WIFI_AUTH_OPEN;
                if(hasPassword) {
                    input.requestInput();
                    input.update();
                    showingKeyboard = true;
                    updated = false;
                } else {
                    connect();
                    updated = false;
                }
                
                #ifdef _PRINTF_INFO_
                printf("Connecting to: %s\n", WiFi.SSID(selectedNetwork).c_str());
                #endif
                
                // Here you would normally implement connection logic
                // WiFi.begin(WiFi.SSID(selectedNetwork).c_str(), password);
                // But for now we just show a message
            }
        }

        if(updated && !showingKeyboard)
            render();
        
        if(last3Button != buttonPressed(3)) {
            last3Button = buttonPressed(3);
            if(!last3Button) {
                viewingOptPage = Options_Count;
                return;
            }
        }
    }
};

class OptionsScene
{
public:
    CnavasAPI *api;
    KeyHelper *keyHelper;
    
    OptionsSceneCursor cursor = Options_WiFi;
    AnimatedData yOffset = AnimatedData(0, 50);
    AnimatedData opacity = AnimatedData(100, 50);
    
    bool last0Button = false;
    bool last1Button = false;
    bool last2Button = false;
    bool last3Button = false;
    
    RepeatedButton button0;
    RepeatedButton button1;
    WiFiScene *wifiScene;
    
    OptionsScene(CnavasAPI *api)
    {
        this->api = api;
        this->keyHelper = new KeyHelper(api);
        this->wifiScene = new WiFiScene(api);
    }
    
    void renderOption(OptionsSceneCursor optionType, int y, bool selected)
    {
        if (selected)
        {
            api->drawRect(10 - 3, y - 3, CANVAS_WIDTH - 20 + 6, 50 + 6, COLOR_OSU);
            api->drawRect(10, y, CANVAS_WIDTH - 20, 50, rgb_darken(255, 255, 255, opacity.current() / 100));
        }
        else
        {
            api->drawRect(10, y, CANVAS_WIDTH - 20, 50, rgb_darken(255, 255, 255, 0.3));
        }
        
        if (optionType == Options_WiFi)
        {
            api->drawTextTopLeft(15, y + 15, "WiFi Settings", COLOR_BLACK);
            api->drawTextTopLeftSmaller(25, y + 33, "Configure network connection", COLOR_BLACK);
        }
        else if (optionType == Options_Bluetooth)
        {
            api->drawTextTopLeft(15, y + 15, "Bluetooth Settings", COLOR_BLACK);
            api->drawTextTopLeftSmaller(25, y + 33, "Configure Bluetooth devices", COLOR_BLACK);
        }
    }
    
    void render()
    {
        api->clear();
        api->drawRect(0, 0, CANVAS_WIDTH, 40, rgb(10, 50, 50));
        api->drawTextTopLeft(10, 20, "Options", COLOR_WHITE);
        
        int baseY = 50 + yOffset.current();
        
        // Render WiFi option
        renderOption(Options_WiFi, baseY, cursor == Options_WiFi);
        
        // Render Bluetooth option
        renderOption(Options_Bluetooth, baseY + 60, cursor == Options_Bluetooth);
        
        keyHelper->setBtn(true, true, true, true);
        keyHelper->setBtnText("Up", "Down", "Select", "Back");
        keyHelper->render();
    }

    OptionsSceneCursor lastScene = Options_Count;
    
    void update(bool forceRender)
    {
        if(viewingOptPage == Options_WiFi)
        {
            bool updated = lastScene != viewingOptPage || forceRender;
            lastScene = viewingOptPage;
            wifiScene->update(updated);
            return;
        }
        bool updated = forceRender;
        updated |= yOffset.update();
        updated |= opacity.update();
        updated |= lastScene != viewingOptPage;
        
        if (updated) {
            lastScene = viewingOptPage;
            render();
        }
        
        if (button0.get(buttonPressed(0)))
        {
            if (cursor > 0)
            {
                cursor = (OptionsSceneCursor)(cursor - 1);
                yOffset.set(10);
                yOffset.setTarget(0);
                opacity.set(60);
                opacity.setTarget(100);
            }
        }
        
        if (button1.get(buttonPressed(1)))
        {
            if (cursor < Options_Count - 1)
            {
                cursor = (OptionsSceneCursor)(cursor + 1);
                yOffset.set(-10);
                yOffset.setTarget(0);
                opacity.set(60);
                opacity.setTarget(100);
            }
        }
        
        if (last2Button != buttonPressed(2))
        {
            last2Button = buttonPressed(2);
            if (!last2Button)
            {
                // Handle option selection
                if (cursor == Options_WiFi)
                {
                    // Handle WiFi option selection
                    #ifdef _PRINTF_INFO_
                    printf("WiFi Settings selected\n");
                    #endif
                    viewingOptPage = Options_WiFi;
                }
                else if (cursor == Options_Bluetooth)
                {
                    // Handle Bluetooth option selection
                    #ifdef _PRINTF_INFO_
                    printf("Bluetooth Settings selected\n");
                    #endif
                }
            }
        }
        
        if (last3Button != buttonPressed(3))
        {
            last3Button = buttonPressed(3);
            if (!last3Button)
            {
                currentScene = Scene::Title;
                return;
            }
        }
    }
};