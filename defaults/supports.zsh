
# Note: All those options concern build-time *only*, and are here to set
#       default values to avoid having the packagers to re-export them in 
#       each recipe. In fact, this file should not concern users outside
#       very rare and special occasions.

supports=(
	cross           # Disable if native-compilation only.
	multithread     # Disable if make -j1, or single-thread.
)

