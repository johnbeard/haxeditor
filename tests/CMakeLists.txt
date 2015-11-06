
add_library(haxtest                 
    haxtest.cpp                     
)                                   
                                    
target_include_directories(haxtest  
    PUBLIC ${GTEST_INCLUDE_DIRS}    
)                                   
                                    
target_link_libraries(haxtest       
    gtest                           
)
