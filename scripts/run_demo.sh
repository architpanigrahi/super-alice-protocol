mkdir -p ../build && cd ../build && cmake .. && cmake --build . 
cd ../scripts
screen -S bootstrap -dm sh start_bootstrap.zsh
screen -S sat2 -dm sh sat2.zsh
screen -S satellite -dm sh start_satellite.zsh
screen -S edge -dm sh start_edge.zsh
screen -S edge2 -dm sh edge2.zsh
tail -f *.log