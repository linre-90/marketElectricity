#include <stdbool.h>
#include "ui.h"
#include "styles.h"
#include "m_utils.h"

/* ############### forward dec ############### */
int applyPadding(int coordinate, int direction);
void drawHours(int offsetX, int posY, ViewModel* viewModel);

/* ############# Implementations ############# */

void initGui(const char* applicationName) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, applicationName);
    SetTargetFPS(2);
}

void stopGui(void) {
    CloseWindow();
}

void drawGui(ViewModel* viewModel, time_t* dateTime, const char* version, bool isLoading) {
    BeginDrawing();
    drawGuiBackground();
    
    if (isLoading) {
        drawLoadingGui();
    }
    else {
        drawGuiTime(dateTime);
        drawGuiPrices(viewModel);
        drawGuiHistory(viewModel, dateTime);
        drawGuiVersion(version);
    }

    EndDrawing();
}

void drawGuiBackground(void) {
    ClearBackground(RAYWHITE);
}

void drawGuiTime(time_t* t) {
    int timeBoxY = applyPadding(0, 1);
    int xCoordinateA = applyPadding(0, 1);
    int yCoordinateB = timeBoxY + FONT_MD;
    t = time(NULL);

    DrawText(TextFormat("Utc:     %s", asctime(gmtime(&t))), xCoordinateA, timeBoxY, FONT_MD, BLACK);
    DrawText(TextFormat("Local:   %s", asctime(localtime(&t))), xCoordinateA, yCoordinateB, FONT_MD, BLACK);
}

void drawGuiPrices(ViewModel* viewModel) {
    // This is where "price box" begins
    int pricesTopPx = 70;
    
    // Calculate draw locations
    int xCoordinateA = applyPadding(0, 1);
    int xCoordinateB = applyPadding(230, 1);
    int yCoordinateA = applyPadding(pricesTopPx + FONT_LG, 0);
    int yCoordinateB = applyPadding(yCoordinateA + FONT_MD, 0);

    // Draw header
    DrawText("Prices", xCoordinateA, pricesTopPx, FONT_LG, BLACK);
   

    // Draw current price
    DrawText("Current hour price:", xCoordinateA, yCoordinateA, FONT_MD, GREEN);
    DrawText(TextFormat("%.2f", viewModel->currHour), xCoordinateB, yCoordinateA, FONT_MD, GREEN);
    // Draw update time
    time_t lastUpdate = viewModel->nextDataUpdateStamp;
    DrawText(TextFormat("Next refresh: %02d:%02d", localtime(&lastUpdate)->tm_hour, localtime(&lastUpdate)->tm_min), xCoordinateB + 50, yCoordinateA + FONT_SM / 2, FONT_SM, Fade(BLACK, .5f));
    // Draw next hour price
    DrawText("Next hour price:   ", xCoordinateA, yCoordinateB, FONT_MD, DARKGREEN);
    DrawText(TextFormat("%.2f", viewModel->nextHour), xCoordinateB, yCoordinateB, FONT_MD, DARKGREEN);

}

void drawGuiHistory(ViewModel* viewModel, time_t* t) {
    // Graph rect area
    Rectangle plottingRec = { 
        .x = PADDING, 
        .y = 200,
        .width = SCREEN_WIDTH - (PADDING * 2), 
        .height = 300 
    };
    int plottingOrigo = plottingRec.y + plottingRec.height;

    // Draw header
    DrawText("History", applyPadding(0, 1), plottingRec.y - (FONT_MD + FONT_LG), FONT_LG, BLACK);
    // Draw bgr rectangle
    DrawRectangleRec(plottingRec, Fade(GREEN, 0.5f));

    // Draw Hours
    int hourListY = plottingRec.y - FONT_SM;
    int hourListXOffset = plottingRec.width / 24;
    drawHours(hourListXOffset , hourListY, viewModel);

    // price points
    float priceInCents = 0.f;
    Vector2 points[24] = { 0 };
    Vector2 verticalLinePoints[2] = { 0 };

    for (int i = 0; i < 24; i++)
    {
        // clamp prices to graph scale
        priceInCents = clamp(viewModel->history[i] * 100, 0, plottingRec.height);

        // Data point on the graph
        Vector2 pricePoint = { 0 };
        pricePoint.x = hourListXOffset * i + PADDING;
        pricePoint.y = plottingOrigo - priceInCents;

        // Vertical line from price located at the bottom of the graph to point
        Vector2 verticalLineStart = { 0 };
        verticalLineStart.x = pricePoint.x;
        verticalLineStart.y = plottingRec.height + plottingRec.y;

        verticalLinePoints[0] = verticalLineStart;
        verticalLinePoints[1] = pricePoint;

        points[i] = pricePoint;

        // Draw price to point line
        DrawSplineLinear(verticalLinePoints, 2, 1.f, Fade(GRAY, .5f));

        // Draw time to point line
        verticalLineStart.y = plottingRec.y;
        verticalLinePoints[0] = verticalLineStart;
        DrawSplineLinear(verticalLinePoints, 2, 1.f, Fade(BLUE, .15f));

        // Draw line point
        DrawCircleLinesV(pricePoint, 4.f, DARKBLUE);
        // Draw price
        DrawText(TextFormat("%.2f", viewModel->history[i]), pricePoint.x, plottingRec.height + plottingRec.y, FONT_SM, BLACK);
    }

    // Draw now line
    Vector2 nowLine = { 0 };
    nowLine.x = (plottingRec.x + plottingRec.width) - 4;
    nowLine.y = plottingOrigo;
    verticalLinePoints[0] = nowLine;

    nowLine.x = (plottingRec.x + plottingRec.width) - 4;
    nowLine.y = plottingRec.y - 15;
    verticalLinePoints[1] = nowLine;
    DrawSplineLinear(verticalLinePoints, 2, 4.f, DARKGREEN);
    DrawText("Now", (plottingRec.x + plottingRec.width) - FONT_MD * 2, plottingRec.y - FONT_MD * 2, FONT_MD, DARKGREEN);

    // Draw spline
    DrawSplineLinear(points, 24, 2.5f, Fade(BLUE, .75f));
}

void drawHours(int offsetX, int posY, ViewModel* viewModel) {
    for (int i = 0; i < 24; i++)
    {
        DrawText(TextFormat("%02d:00", viewModel->hourList[i]), offsetX * i + PADDING, posY, FONT_SM, BLACK);
    }
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
