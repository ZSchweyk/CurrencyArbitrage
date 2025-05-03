from pprint import pprint

from currency_api_wrapper import get_exchange_rates
from arb_lib import find_arb_ops



def create_exchange_rate_map(currencies: set):
    return {currency: get_exchange_rates(currency, currs=currencies-{currency}) for currency in currencies}

def main():
    currencies = {"eur", "usd", "gbp", "brl", "cny", "jpy"}
    map = create_exchange_rate_map(currencies)
    # pprint(map)
    pprint(find_arb_ops(map))



if __name__ == "__main__":
    main()



