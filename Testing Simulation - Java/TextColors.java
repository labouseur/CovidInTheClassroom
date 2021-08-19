//Functions: Allows the console to be color coded in such a way that may help user interaction
//Partial pull from https://www.codegrepper.com/code-examples/actionscript/ansi+colors

public class TextColors {
    // Reset formatting
    public static final String RESET = "\033[0m";  // Text Reset

    public static final String BLACK = "\033[0;30m";  
    public static final String RED = "\033[0;31m";     
    public static final String GREEN = "\033[0;32m";   
    public static final String YELLOW = "\033[0;33m";  
    public static final String BLUE = "\033[0;34m";   
    public static final String PURPLE = "\033[0;35m"; 
    public static final String CYAN = "\033[0;36m";   
    public static final String WHITE = "\033[0;37m";  

    public static final String BLACK_BOLD = "\033[1;30m";  
    public static final String RED_BOLD = "\033[1;31m"; 
    public static final String GREEN_BOLD = "\033[1;32m";  
    public static final String YELLOW_BOLD = "\033[1;33m"; 
    public static final String BLUE_BOLD = "\033[1;34m";  
    public static final String PURPLE_BOLD = "\033[1;35m"; 
    public static final String CYAN_BOLD = "\033[1;36m";  
    public static final String WHITE_BOLD = "\033[1;37m"; 

    public static final String BLACK_BRIGHT = "\033[0;90m"; 
    public static final String RED_BRIGHT = "\033[0;91m";  
    public static final String GREEN_BRIGHT = "\033[0;92m"; 
    public static final String YELLOW_BRIGHT = "\033[0;93m"; 
    public static final String BLUE_BRIGHT = "\033[0;94m";  
    public static final String PURPLE_BRIGHT = "\033[0;95m";
    public static final String CYAN_BRIGHT = "\033[0;96m"; 
    public static final String WHITE_BRIGHT = "\033[0;97m"; 

    public static final String WHITE_UNDERLINED = "\033[4;37m"; 

    //Pre-determined formats for output
    public static void message(String text){
        System.out.println(CYAN + text + RESET);

    }

    public static void warning(String text){
        System.out.println(YELLOW + text + RESET);

    }

    public static void error(String text){
        System.out.println(RED_BOLD + text + RESET);

    }

    public static void verified(String text){
        System.out.println(GREEN_BRIGHT + text + RESET);

    }

    public static void welcome(String text){
        System.out.println(RED_BRIGHT + text + RESET);

    }

    public static String red(String text){
        return RED + text + RESET;

    }

    public static String green(String text){
        return GREEN_BRIGHT + text + RESET;

    }

    public static String yellow(String text){
        return YELLOW + text + RESET;

    }

    public static void bannerBreak(){
        System.out.println("--------------------------------------------------------");

    }
}
