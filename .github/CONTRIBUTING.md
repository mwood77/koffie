# How to contribute

First of all, welcome! 

I'm really glad you're reading this, because we need volunteers to help adapt this project to other espresso machines.

## Submitting changes (documentation & code)

Please send a [GitHub Pull Request to our maintainers](https://github.com/mwood77/koffie/pull/new/master) with a clear list of what you've done. 
- If you're unfamilair with pull requests, you can [read more about them here](http://help.github.com/pull-requests/). 

We only accept clear log messages for your commits. One-line messages are fine for small changes, but bigger changes should look like this:

    $ git commit -m "A brief summary of the commit
    > 
    > A paragraph describing what changed and its impact."
    
    
## Coding conventions

> If you read through the code, you'll probably get the gist it. 
>
> I come from a JS/Java background, so I recognize it isn't the most conventional C++ syntax.

Readability is the most important factor, and that is what I optimize for:

- We indent using single tabs (4 spaces)
- We ALWAYS put spaces after list items and method parameters (`[1, 2, 3]`, not `[1,2,3]`), around operators (`x += 1`, not `x+=1`), and around hash arrows.
- Please remember that this is open source software. Consider the people who will read your code, and make it look nice for them. 
  - It's sort of like driving a car: Perhaps you love doing donuts when you're alone, but with passengers the goal is to make the ride as smooth as possible.
 
