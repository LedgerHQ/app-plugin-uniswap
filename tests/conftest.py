import pytest
from ragger.conftest import configuration
from .utils import WalletAddr


###########################
### CONFIGURATION START ###
###########################

# You can configure optional parameters by overriding the value of ragger.configuration.OPTIONAL_CONFIGURATION
# Please refer to ragger/conftest/configuration.py for their descriptions and accepted values

configuration.OPTIONAL.MAIN_APP_DIR = "tests/.test_dependencies/"

configuration.OPTIONAL.BACKEND_SCOPE = "session"


#########################
### CONFIGURATION END ###
#########################

# Pull all features from the base ragger conftest using the overridden configuration
pytest_plugins = ("ragger.conftest.base_conftest", )

@pytest.fixture
def wallet_addr(backend):
    return WalletAddr(backend)

from .abi_reader import read_uniswap_contract_data
@pytest.fixture(scope=configuration.OPTIONAL.BACKEND_SCOPE)
def uniswap_contract_data():
    return read_uniswap_contract_data()

from .uniswap_client import UniswapClient
@pytest.fixture(scope=configuration.OPTIONAL.BACKEND_SCOPE)
def uniswap_client(backend, uniswap_contract_data):
    return UniswapClient(backend, uniswap_contract_data)

from .navigation_helper import NavigationHelper
@pytest.fixture(scope="function")
def navigation_helper(navigator, backend, test_name):
    return NavigationHelper(navigator, backend, test_name)
