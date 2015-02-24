

### What happens if you request state and it's invalid?

- When you get a given state of an interface, if that interface has ever received the kind of data you want, it gives you the last info it received (with timestamp, of course).
- If it hasn't ever received that type of data, then it tells you so by returning failure.