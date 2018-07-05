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

```java
public interface StoryService {
	List<Story> findAllStory();
	List<Story> findStoryById(String ID);
	Boolean saveStory(Story story);
	Boolean deleteStory(String ID);
}
```

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
