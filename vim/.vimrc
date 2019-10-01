if &compatible
	set nocompatible
endif	

syntax on

filetype plugin on
filetype indent off

augroup remember_folds
	autocmd!
	autocmd BufWinLeave * mkview
	autocmd BufWinEnter * silent! loadview
augroup END

let g:loaded_matchparen=1

set backspace=indent,eol,start
set history=200
set laststatus=2
set wildmenu
set autoindent
set noexpandtab
set tabstop=4
set shiftwidth=4
set softtabstop=4
set wrap
set linebreak
set breakindent
set noshowmode
set shortmess+=F
set noswapfile

let g:onedark_termcolors=16
colorscheme onedark

"Search
set hlsearch
set incsearch
set ignorecase
set smartcase

highlight Normal ctermbg=NONE
highlight nonText ctermbg=NONE
highlight EndOfBuffer ctermfg=0
highlight FoldColumn ctermbg=0 ctermfg=0
highlight VertSplit ctermfg=0 ctermbg=0

"let g:limelight_conceal_ctermfg = 15

nmap <F9> za
nmap <F8> :set syntax=whitespace<CR>
nmap <F2> :set number!<CR>
imap <C-n> <Esc>:w<CR>i
imap jk <Esc>
imap kj <Esc>
nmap oo o<Esc>
nmap OO O<Esc>
nmap <F1> :noh<CR>
