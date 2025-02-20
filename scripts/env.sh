alias mkf="cd $(git rev-parse --show-toplevel)/build && make && make install; cd -"
alias mk_clean="deactivate && cd $(git rev-parse --show-toplevel) && rm -rf build && rm -rf .venv; cd -"
source .venv/bin/activate