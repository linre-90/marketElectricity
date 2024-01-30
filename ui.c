#include <stdbool.h>
#include "ui.h"
#include "styles.h"
#include "m_utils.h"
#include "web.h"
#include "m_utils.h";

/* ############### forward dec ############### */
int applyPadding(int coordinate, int direction);
//void drawHours(int offsetX, int posY, ViewModel* viewModel);

/* ############# Implementations ############# */

void initGui(const char* applicationName) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, applicationName);
    SetTargetFPS(2);
}

void stopGui(void) {
    CloseWindow();
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
    int timeBoxY = applyPadding(0, 1);
    int xCoordinateA = applyPadding(0, 1);
    int yCoordinateB = timeBoxY + FONT_MD;
    t = time(NULL);

    DrawText(TextFormat("Utc:     %s", asctime(gmtime(&t))), xCoordinateA, timeBoxY, FONT_MD, BLACK);
    DrawText(TextFormat("Local:   %s", asctime(localtime(&t))), xCoordinateA, yCoordinateB, FONT_MD, BLACK);
}

void drawGuiPrices(ViewModel* viewModel) {
    // This is where "price box" begins
    int pricesTopPx = 50;
    
    // Calculate draw locations
    int xCoordinateA = applyPadding(0, 1);
    int xCoordinateB = applyPadding(230, 1);
    int yCoordinateA = applyPadding(pricesTopPx + FONT_LG, 0);
    int yCoordinateB = applyPadding(yCoordinateA + FONT_MD, 0);

    // Draw current price
    DrawText("Current hour price:", xCoordinateA, yCoordinateA, FONT_MD, GREEN);
    DrawText(TextFormat("%.2f cents/KWh", viewModel->priceArr[viewModel->currHourIndex].price), xCoordinateB, yCoordinateA, FONT_MD, GREEN);
    // Draw update time
    time_t lastUpdate = viewModel->nextDataUpdateStamp;
    DrawText(TextFormat("Next refresh: %02d:%02d", localtime(&lastUpdate)->tm_hour, localtime(&lastUpdate)->tm_min), xCoordinateB + 200, yCoordinateA + FONT_SM / 2, FONT_SM, Fade(BLACK, .5f));
    // Draw next hour price
    DrawText("Next hour price:   ", xCoordinateA, yCoordinateB, FONT_MD, DARKGREEN);
    if (viewModel->NextHourIndex  != -1) {
        DrawText(TextFormat("%.2f cents/KWh", viewModel->priceArr[viewModel->NextHourIndex].price), xCoordinateB, yCoordinateB, FONT_MD, DARKGREEN);
    }
    else {
        DrawText("Error fetching next price index out of bounds.", xCoordinateB, yCoordinateB, FONT_MD, DARKGREEN);

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
    int plottingOrigo = plottingRec.y + plottingRec.height;
    int hourListXOffset = plottingRec.width / 24;

    // Draw header
    DrawText(TextFormat("%2d.%2d prices cents/KWh", gmtime(&t)->tm_mday, gmtime(&t)->tm_mon + 1), applyPadding(0, 1), plottingRec.y - (FONT_MD + FONT_LG) - 20, FONT_LG, BLACK);
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
        int DD = gmtime(&viewModel->priceArr[dataIter].utcTime)->tm_mday;
        int MM = gmtime(&viewModel->priceArr[dataIter].utcTime)->tm_mon;
        int YYYY = gmtime(&viewModel->priceArr[dataIter].utcTime)->tm_year;
        int cDD = gmtime(&t)->tm_mday;
        int cMM = gmtime(&t)->tm_mon;
        int cYYYY = gmtime(&t)->tm_year;

        // Find this days prices
        if (DD == cDD && MM == cMM && YYYY == cYYYY) {
            // Plot data circles
            clampedPrice = clamp(viewModel->priceArr[dataIter].price, 0, plottingRec.height);
            
            // Draw hour marker
            DrawText(TextFormat("%02d", localtime(&viewModel->priceArr[dataIter].utcTime)->tm_hour), hourListXOffset * plottingIter + PADDING, plottingOrigo, FONT_SM, BLACK);
            // Draw bar
            Rectangle barRec = {
                .x = hourListXOffset * plottingIter + PADDING,
                .y = plottingOrigo - clampedPrice,
                .width = 10,
                .height = clampedPrice
            };
            DrawRectangleRec(barRec, BLUE);
            // Draw price
            DrawText(TextFormat("%.2f", viewModel->priceArr[dataIter].price), barRec.x, barRec.y - FONT_SM, FONT_SM, BLACK);
            average_sum += viewModel->priceArr[dataIter].price;
            average_count++;
            plottingIter++;
        }
        dataIter--;
    }

    // Draw average day price
    DrawText(TextFormat("Day average: %.2f cents/KWh", average_sum / average_count), applyPadding(0, 1), plottingRec.y - FONT_MD - 20, FONT_MD, BLACK);
}


void drawLoadingGui(void) {
    DrawText("... Loading ...", SCREEN_WIDTH / 2 - 130 , SCREEN_HEIGHT / 2 - FONT_LG, FONT_LG *2, BLACK);
}

void drawGuiVersion(const char* version) {
    int xPosition = applyPadding(SCREEN_WIDTH - 50, -1);
    int yPosition = applyPadding(SCREEN_HEIGHT - FONT_SM, -1);
    DrawText(version, xPosition, yPosition, FONT_SM, GRAY);
}

int applyPadding(int coordinate, int direction) {
    return coordinate + (PADDING * direction);
}
