#!/bin/bash -e

# Internal-only setup steps

git config --global url.ssh://git@bitbucket.sw.nxp.com/genavbtsn/bifrost.git.insteadOf https://github.com/nxp-zephyr/bifrost

west update bifrost
python3 "$WORKSPACE_DIR/bifrost/scripts/init_env.py"
