#include <stdbool.h>
#include "raylib.h"
#include "ui.h"
#include "styles.h"
#include "m_utils.h"
#include "web.h"
#include "viewModel.h"

/* ############### globals ############### */

/* Application font */
Font normalFont;
Font smallFont;

/* ############### forward dec ############### */

/* Applies default padding to coordinates in direction indicated by direction.*/
float applyPadding(int coordinate, int direction);

/* ############# Implementations ############# */

void initGui(const char* applicationName) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, applicationName);
    normalFont = LoadFontEx("resources/normal_font.ttf", 20, 0, 250);
    smallFont = LoadFontEx("resources/normal_font.ttf", 14, 0, 250);
    SetTextLineSpacing(48);
    SetTargetFPS(2);
}

void stopGui(void) {
    UnloadFont(smallFont);
    UnloadFont(normalFont);
    CloseWindow();
}

bool detectWindowShouldClose() {
    return WindowShouldClose();
}

void drawGui(ViewModel* viewModel, const char* version, bool isLoading) {
    BeginDrawing();
    drawGuiBackground();
    
    if (isLoading) {
        drawLoadingGui();
    }
    else {
        drawGuiTime();
        drawGuiPrices(viewModel);
        drawGuiDatePrices(viewModel);
        drawGuiVersion(version);
    }

    EndDrawing();
}

void drawGuiBackground(void) {
    ClearBackground(RAYWHITE);
}

void drawGuiTime() {
    time_t t = time(NULL);
    // Setup coordinates
    float timeBoxY = applyPadding(0, 1);
    float xCoordinateA = applyPadding(0, 1);
    float yCoordinateB = timeBoxY + FONT_MD;
    // Draw the stuff
    DrawTextEx(normalFont, TextFormat("Utc:      %s", asctime(gmtime(&t))), (Vector2) { xCoordinateA, timeBoxY }, FONT_MD, 2,BLACK);
    DrawTextEx(normalFont, TextFormat("Local:   %s", asctime(localtime(&t))), (Vector2) { xCoordinateA, yCoordinateB}, FONT_MD, 2, BLACK);
}

void drawGuiPrices(ViewModel* viewModel) {
    // This is where "price box" begins from window top
    int pricesTopPx = 50;
    
    // Calculate draw locations
    float xCoordinateA = applyPadding(0, 1);
    float xCoordinateB = applyPadding(230, 1);
    float yCoordinateA = applyPadding(pricesTopPx + FONT_LG, 0);
    float yCoordinateB = applyPadding(yCoordinateA + FONT_MD, 0);

    // Draw current price
    DrawTextEx(normalFont, "Current hour price:", (Vector2) { xCoordinateA, yCoordinateA}, FONT_MD, 2, BLACK);
    DrawTextEx(normalFont, TextFormat("%.2f cents/KWh", viewModel->priceArr[viewModel->currHourIndex].price), (Vector2) { xCoordinateB, yCoordinateA}, FONT_MD, 2, BLACK);
    // Draw update time
    time_t lastUpdate = viewModel->nextDataUpdateStamp;
    DrawTextEx(smallFont, TextFormat("Next refresh: %02d:%02d", localtime(&lastUpdate)->tm_hour, localtime(&lastUpdate)->tm_min), (Vector2) { xCoordinateB + 200, yCoordinateA }, FONT_SM, 2, Fade(BLACK, .5f));
    // Draw next hour price
    DrawTextEx(normalFont, "Next hour price:   ", (Vector2) { xCoordinateA, yCoordinateB }, FONT_MD, 2, BLACK);
    if (viewModel->NextHourIndex  != -1) {
        DrawTextEx(normalFont, TextFormat("%.2f cents/KWh", viewModel->priceArr[viewModel->NextHourIndex].price), (Vector2) { xCoordinateB, yCoordinateB }, FONT_MD, 2, BLACK);
    }
    else {
        DrawTextEx(normalFont, "Error fetching next price index out of bounds.", (Vector2) { xCoordinateB, yCoordinateB }, FONT_MD, 2, RED);
    }
}

void drawGuiDatePrices(ViewModel* viewModel) {
    time_t t = time(NULL);
    // Graph rect area
    Rectangle plottingRec = { 
        .x = PADDING, 
        .y = 200,
        .width = SCREEN_WIDTH - (PADDING * 2), 
        .height = 300 
    };
    // Graph setup
    float plottingOrigo = plottingRec.y + plottingRec.height;
    float hourListXOffset = plottingRec.width / 24;

    // Draw header
    DrawTextEx(normalFont, TextFormat("%2d.%d prices cents/KWh", gmtime(&t)->tm_mday, gmtime(&t)->tm_mon + 1), (Vector2) { applyPadding(0, 1), plottingRec.y - FONT_MD * 2 }, FONT_MD, 2, BLACK);
    // Draw bgr rectangle
    DrawRectangleRec(plottingRec, Fade(GREEN, 0.5f));

    // Start plotting from data end
    float clampedPrice = 0.f;
    int dataIter = NUM_OF_API_RESULTS - 1;
    int plottingIter = 0;
    float average_sum = 0.0f;
    int average_count = 0;
    while (dataIter >= 0) {
        // gmtime comparisons dont work in if statement so they are extracted here...i am confused...
        int DD = localtime(&viewModel->priceArr[dataIter].utcTime)->tm_mday;
        int MM = localtime(&viewModel->priceArr[dataIter].utcTime)->tm_mon;
        int YYYY = localtime(&viewModel->priceArr[dataIter].utcTime)->tm_year;
        int cDD = localtime(&t)->tm_mday;
        int cMM = localtime(&t)->tm_mon;
        int cYYYY = localtime(&t)->tm_year;

        // Find current day prices
        if (DD == cDD && MM == cMM && YYYY == cYYYY) {
            // Clamp price so data fits to area, range is 0 - 300 cents
            clampedPrice = clamp(viewModel->priceArr[dataIter].price, 0, plottingRec.height);
            
            // Draw hour marker
            DrawTextEx(smallFont, TextFormat("%02d", localtime(&viewModel->priceArr[dataIter].utcTime)->tm_hour), (Vector2) { hourListXOffset* plottingIter + PADDING, plottingOrigo }, FONT_SM, .75f, BLACK);
            // Draw bar
            Rectangle barRec = {
                .x = (float)hourListXOffset * plottingIter + PADDING,
                .y = (float)plottingOrigo - clampedPrice,
                .width = 10.0f,
                .height = (float)clampedPrice
            };
            DrawRectangleRec(barRec, BLUE);
            // Draw price
            DrawTextEx(smallFont, TextFormat("%.2f", viewModel->priceArr[dataIter].price), (Vector2) { barRec.x, barRec.y - FONT_SM }, FONT_SM, .75f, BLACK);
            // average calculation
            average_sum += viewModel->priceArr[dataIter].price;
            average_count++;
            plottingIter++;
        }
        dataIter--;
    }

    // Draw average day price
    DrawTextEx(smallFont, TextFormat("Graph average: %.2f cents/KWh", average_sum / average_count), (Vector2) { applyPadding(0, 1), plottingRec.y - FONT_MD }, FONT_SM, 2, BLACK);
}


void drawLoadingGui(void) {
    DrawTextEx(normalFont, "... Loading ...", (Vector2) { SCREEN_WIDTH / 2 - 75 , SCREEN_HEIGHT / 2 - FONT_MD}, FONT_MD, 2, BLACK);
}

void drawGuiVersion(const char* version) {
    float xPosition = applyPadding(SCREEN_WIDTH - 100, -1);
    float yPosition = applyPadding(SCREEN_HEIGHT - FONT_SM, -1);
    DrawTextEx(normalFont, version, (Vector2) { xPosition, yPosition }, FONT_MD, 2, GRAY);
}

float applyPadding(int coordinate, int direction) {
    return (float)(coordinate + (PADDING * direction));
}
