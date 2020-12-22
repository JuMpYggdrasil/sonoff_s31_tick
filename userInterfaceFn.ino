// Performed by btnTicker repeatedly.
void btnTick() {
    S31_Button.update();

    //    if (S31_Button.isClick()) {
    //#if USE_TELNET
    //        debugD("Click");
    //#endif
    //    }
    if (S31_Button.isSingleClick()) {
            singleClick_flag = true;
    }
    if (S31_Button.isDoubleClick()) {
            doubleClick_flag = true;
    }
    if (S31_Button.isLongClick()) {
            longClick_flag = true;
    }
}

// Performed by ledTicker repeatedly.
void ledTick() {
    blueLed.update();
}
