# version 330 core                                                                    
                                                                                    
uniform sampler2D gColorMap;                                                        
                                                                                    
in vec4 fColor;   
in vec2 TexCoord;                                                                
out vec4 FragColor;                                                                 
                                                                                    
void main(){                                                                                   
    FragColor = texture(gColorMap, TexCoord) * fColor * fColor.w;
	if (FragColor.r <= 0.25 && FragColor.g <= 0.25 && FragColor.b <= 0.25) 
    {
        discard;
    }      
}