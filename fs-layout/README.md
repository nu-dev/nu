sample filesystem layout for nu.

 - `theme/` - themes. each theme has a listing.html template for the main pages, a post.html template for posts, and a special.html template for special pages. the file style.css will be automatically copied if it exists to `resources/`, provided it does not override anything.
 - `raw/` - raw markdown files
 - `posts/` - compiled files from `raw/` that follow the posting format. if they do not, they are considered special pages and go under a different directory.
 - `special/` - special pages, ie about, that do not follow a post format.
 - `resources/` - resources such as images can be placed here.

it is expected typically that everything is served from the directory, so linking to, say, the first page can be done with `<a href="/">First page</a>`.