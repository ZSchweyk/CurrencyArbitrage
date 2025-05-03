import requests

def get_exchange_rates(base_curr, currs=None, day=None):
	resp = requests.get(f"https://cdn.jsdelivr.net/npm/@fawazahmed0/currency-api@{'latest' if not day else day}/v1/currencies/{base_curr}.json")
	resp_jsn = resp.json()
	if currs is not None:
		return {curr: resp_jsn[base_curr][curr] for curr in currs}
	return resp

