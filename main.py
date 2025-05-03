from pprint import pprint

from currency_api_wrapper import get_exchange_rates
from arb_lib import find_arb_ops



def create_exchange_rate_map(currencies: set):
    return {currency: get_exchange_rates(currency, currs=currencies-{currency}) for currency in currencies}

def main():
    currencies = {"eur", "usd", "gbp", "brl", "cny", "jpy"}
    # map = create_exchange_rate_map(currencies)
    # pprint(map)
    map = {
        "brl": {
            "eur": 0.15384615,  # 1 BRL = 0.15384615 EUR (6.5 BRL/EUR)
            "usd": 0.16666667   # 1 BRL = 0.16666667 USD (6 BRL/USD)
        },
        "eur": {
            "brl": 6.5,         # 1 EUR = 6.5 BRL
            "usd": 1.1          # 1 EUR = 1.1 USD
        },
        "usd": {
            "eur": 0.95,        # 1 USD = 0.95 EUR (Key arbitrage rate)
            "brl": 6.0          # 1 USD = 6.0 BRL
        }
    }
    pprint(find_arb_ops(map))



if __name__ == "__main__":
    main()



