```java
public class StoryController {
	
	private final Logger logger = LoggerFactory.getLogger(this.getClass());
	@Autowired

	private DiscoveryClient discoveryClient;

  	@Bean
  	public RestTemplate restTemplate() {
       		return new RestTemplate();
  	}
  	@Value("${eureka.instance.instance-id}")
	String instance_id;
	
	@Autowired
	RestTemplate restTemplate;
	
	@Autowired
	StoryService storyService;

	@HystrixCommand(commandKey = "story-service", fallbackMethod = "getAllStoryFallback")
	@RequestMapping(value = "/story", method = RequestMethod.GET)
	public ResponseEntity<List<Story>> getAllStory()
	{
		try{
		    	Optional<List<Story>> maybeAllStory = Optional.of(storyService.findAllStory());
		    	return new ResponseEntity<List<Story>>(maybeAllStory.get(), HttpStatus.OK);
        	}catch(Exception e)
        	{
            		return new ResponseEntity<List<Story>>(HttpStatus.NOT_FOUND);
        	}
	}
	
    	public ResponseEntity<List<Story>>  getAllStoryFallback()
    	{
        	try{
            		Optional<List<ServiceInstance>> maybeServiceInstance 
                		= Optional.of(this.discoveryClient.getInstances("story-service"));
        
            	List<ServiceInstance> Instance 
                	= maybeServiceInstance.get().stream()
                                            	.filter(service -> 
                                                    	!instance_id.equals(service.getServiceId()))
                                            	.collect(Collectors.toList());

            	ServiceInstance service = Instance.get(0);
        
            	URI uri = URI.create(service.getHost() + ":" + service.getPort() + "/story");

            	ResponseEntity <List<Story>> rest =
				    	restTemplate.exchange(uri, HttpMethod.GET,null, new
					ParameterizedTypeReference<List<Story>>() {});
            
            	return new ResponseEntity<List<Story>>(rest.getBody(), HttpStatus.OK);

        	}catch(Exception e)
        	{
			  e.printStackTrace();
        	}
        	return null;
    	}
```
sdf
sdf

```java
public interface StoryService {
	List<Story> findAllStory();
	List<Story> findStoryById(String ID);
	Boolean saveStory(Story story);
	Boolean deleteStory(String ID);
}
```
sdfs
sdf

```java
@Service("storyService")
public class StoryServiceImpl implements StoryService {

	private Logger logger = LoggerFactory.getLogger(this.getClass());

	@Autowired
	private StoryRepository storyRepository;
	
	@Override
	public List<Story> findAllStory()
	{
		Optional<List<Story>> maybeStoryIter =
			Optional.ofNullable(storyRepository.findAllStory(PageRequest.of(0,15)));
			
		return maybeStoryIter.get();
	}
}
```

sddsf
sdf

```java
public interface StoryRepository extends CrudRepository<Story, String> {

	@Query("SELECT s FROM Story s ORDER BY s.story_id DESC")
	List<Story> findAllStory(Pageable pageable);
	}
```



```java
@Entity
public class Story {

	@Id
	@GeneratedValue(strategy=GenerationType.AUTO)
	private int story_id;
	private String ID;
	private String message;
	
	protected Story(){}
	
	public Story(String ID, String message){
	
		this.ID=ID;
		this.message=message;
	}
	public int getStory_id() { return story_id; }
	public String getID() { return ID; }
	public void setID(String ID) { ID = ID; }
	public String getMessage() { return message; }
	public void setMessage(String message) { this.message = message;}
}
```
