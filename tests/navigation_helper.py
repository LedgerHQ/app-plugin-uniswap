from pathlib import Path
from ragger.navigator import NavInsID

ROOT_SCREENSHOT_PATH = Path(__file__).parent

class NavigationHelper:
    def __init__(self, navigator, firmware, test_name):
        self.navigator = navigator
        self.firmware = firmware
        self.test_name = test_name

    def ui_validate(self):
        if self.firmware.device.startswith("nano"):
            navigation_method = NavInsID.RIGHT_CLICK
            validation_method = [NavInsID.BOTH_CLICK]
            text_to_search = "Accept"
        else:
            navigation_method = NavInsID.USE_CASE_REVIEW_TAP
            validation_method = [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS]
            text_to_search = "Hold to sign"

        self.navigator.navigate_until_text_and_compare(navigation_method,
                                                       validation_method,
                                                       text_to_search,
                                                       ROOT_SCREENSHOT_PATH,
                                                       self.test_name)
