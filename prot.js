
const valid_chars = 'abc';
word_size = 2;


word = '';
for(var i = 0; i < valid_chars.length; i++){
for(var i = 0; i < valid_chars.length; i++){
    for(var j = 0 ; j <= word_size; j++) {
        word[j] = valid_chars[i];
        console.log("...",valid_chars[i]);
        console.log("..",j);
        console.log(".",word);        
    }
}
}