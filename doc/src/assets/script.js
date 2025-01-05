(function () {
  let search_result = document.querySelector('.md-search-result');
  let search_scrollwrap = document.querySelector('.md-search__scrollwrap');
  new MutationObserver(function () {
    search_scrollwrap.style.height = search_result.offsetHeight + 'px';
  }).observe(search_result, { childList: true, subtree: true });
})();